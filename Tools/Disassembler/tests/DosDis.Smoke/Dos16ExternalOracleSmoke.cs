using System.Text.Json;
using DosDis.Core;

/// <summary>
/// Synthetic contract tests for the EXTERNAL-oracle comparison interface.
/// Expected values here are hand-constructed from known x86 opcodes; these
/// cases do NOT constitute a Capstone/Ghidra execution or G2 acceptance.
/// </summary>
internal static class Dos16ExternalOracleSmoke
{
    private static void Equal<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception($"{label}: expected {expected}, got {actual}");
    }

    private static void Denied(Action action, string label)
    {
        try { action(); }
        catch (InvalidDataException) { return; }
        throw new Exception(label + ": invalid external oracle was accepted");
    }

    public static void Run()
    {
        // JMP SHORT +2 at COM 0100 reaches RET at 0104; the two NOP
        // bytes are not discovered by an entry-only candidate traversal.
        var bytes = new byte[] { 0xEB, 0x02, 0x90, 0x90, 0xC3 };
        var artifact = BinaryArtifact.FromBytes(bytes, "synthetic-oracle.com");
        var region = DosComImage.LoadExplicit(artifact).CreateCodeRegion();
        var graph = Dos16ControlFlowAnalysis.Analyze(region, 0x100);
        var manifest = new Dos16ExternalOracleManifest(
            1, "synthetic", "COM", artifact.Sha256, artifact.Length,
            region.FileOffset, region.ByteLength, region.Segment, region.Origin,
            "independently-constructed-fixture", "synthetic-v1",
            "16-bit real mode; manually derived from ISA",
            "external-expected-values-not-yet-human-reviewed",
            new[] {
                new Dos16ExternalExpectedInstruction(
                    0x100, 0, "EB02", "UnconditionalBranch", "short-jmp",
                    "branch", 0x104),
                new Dos16ExternalExpectedInstruction(
                    0x104, 4, "C3", "Return", "near-ret")
            });
        var report = Dos16ExternalOracleValidator.Compare(region, graph, manifest);
        Equal(report.Matched, 2, "two independently specified fixture instruction assertions");
        Equal(report.Missing, 0, "short JMP target is present");
        Equal(report.Conflicting, 0, "source-original mapping agrees with reference");
        Equal(report.NotObserved, 0, "both selected fixture instructions discovered");
        Equal(report.UnbenchmarkedCandidates, 0,
            "known-two-entry fixture covers the two observed candidate starts");
        Equal(report.IndependenceStatus, "claimed-external-not-reviewed",
            "manifest name does not independently certify the oracle producer");
        Equal(report.AnalysisStatus, "candidate-comparison-not-certification",
            "candidate comparisons never certify execution or full code recovery");
        Equal(report.SourceSha256, artifact.Sha256, "report binds original source SHA");
        Equal(report.SourceByteLength, artifact.Length,
            "comparison report retains independently measured file length");
        Equal(report.RegionFileOffset, region.FileOffset,
            "report retains original physical mapped base");
        Equal(report.RegionByteLength, region.ByteLength,
            "report retains candidate mapping byte extent");
        Equal(report.Origin, region.Origin,
            "report retains explicit 16-bit IP origin");
        Equal(report.AnalysisEntry, (ushort)0x100,
            "report retains exact candidate CFG entry");
        Equal(report.MappingEvidence, graph.MappingEvidence,
            "report retains analyst mapping assumption and status");
        Equal(report.DecoderConfiguration, manifest.DecoderConfiguration,
            "report preserves unreviewed claimed external decoder settings");

        // Canonical semantic evidence is derived at candidate decode time,
        // never by reparsing formatted assembly, and is only compared
        // when an independent manifest explicitly supplies the assertion.
        // MOV AX,1234; CMP AX,1234; CMP BX,-2 (sign-extended imm8); RET.
        var operandBytes = new byte[] {
            0xB8, 0x34, 0x12,
            0x3D, 0x34, 0x12,
            0x83, 0xFB, 0xFE,
            0xC3
        };
        var operandArtifact = BinaryArtifact.FromBytes(operandBytes,
            "synthetic-operand-oracle.com");
        var operandRegion = DosComImage.LoadExplicit(operandArtifact).CreateCodeRegion();
        var operandGraph = Dos16ControlFlowAnalysis.Analyze(operandRegion, 0x100);
        Dos16InstructionFacts MovAx(uint immediate) =>
            new("Mov", 2, new Dos16OperandFact("Register", "AX"),
                new Dos16OperandFact("Immediate16", ImmediateValue: immediate));
        Dos16InstructionFacts CmpAx() =>
            new("Cmp", 2, new Dos16OperandFact("Register", "AX"),
                new Dos16OperandFact("Immediate16", ImmediateValue: 0x1234));
        Dos16InstructionFacts CmpBxSigned() =>
            new("Cmp", 2, new Dos16OperandFact("Register", "BX"),
                new Dos16OperandFact("Immediate8to16", ImmediateValue: -2));
        var operandManifest = manifest with {
            SourceSha256 = operandArtifact.Sha256,
            SourceByteLength = operandArtifact.Length,
            RegionByteLength = operandRegion.ByteLength,
            ExpectedInstructions = new[] {
                new Dos16ExternalExpectedInstruction(0x100, 0, "B83412",
                    "Next", "external-mov-ax-imm16",
                    ExpectedFacts: MovAx(0x1234)),
                new Dos16ExternalExpectedInstruction(0x103, 3, "3D3412",
                    "Next", "external-cmp-ax-imm16",
                    ExpectedFacts: CmpAx()),
                new Dos16ExternalExpectedInstruction(0x106, 6, "83FBFE",
                    "Next", "external-cmp-bx-imm8-signed",
                    ExpectedFacts: CmpBxSigned()),
                new Dos16ExternalExpectedInstruction(0x109, 9, "C3",
                    "Return", "external-ret")
            }
        };
        var operandMatch = Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, operandManifest);
        Equal(operandMatch.Matched, 4,
            "external canonical mnemonic/register/immediate facts compare");
        Equal(operandMatch.UnbenchmarkedCandidates, 0,
            "operand sample identifies every retained candidate start");
        Equal(operandGraph.Instructions.Single(i =>
            i.Address.Offset == 0x106).DecoderFacts!.Operand1!.ImmediateValue,
            -2L, "Iced-sourced semantic record preserves sign-extended imm8");

        var wrongRegister = operandManifest with {
            ExpectedInstructions = new[] {
                operandManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MovAx(0x1234) with {
                        Operand0 = new Dos16OperandFact("Register", "BX")
                    }
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, wrongRegister).Conflicting, 1,
            "external AX/BX register discrepancy cannot pass by matching bytes");
        var wrongImmediate = operandManifest with {
            ExpectedInstructions = new[] {
                operandManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MovAx(0x1235)
                }
            }
        };
        var immediateMismatch = Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, wrongImmediate);
        Equal(immediateMismatch.Conflicting, 1,
            "external immediate disagreement cannot pass by matching opcode text");
        Equal(immediateMismatch.Findings[0].DetailCode,
            "candidate-structured-operands-disagree-with-external-oracle",
            "operand-value disagreement has a machine-readable finding");
        var wrongSignExtension = operandManifest with {
            ExpectedInstructions = new[] {
                operandManifest.ExpectedInstructions[2] with {
                    ExpectedFacts = CmpBxSigned() with {
                        Operand1 = new Dos16OperandFact(
                            "Immediate8to16", ImmediateValue: 254)
                    }
                }
            }
        };
        Denied(() => Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, wrongSignExtension),
            "a sign-extended imm8 has a canonical -128..127 range");
        var missingFactsGraph = operandGraph with {
            Instructions = operandGraph.Instructions.Select(i =>
                i.Address.Offset == 0x100
                    ? i with { DecoderFacts = null } : i).ToArray()
        };
        var unavailable = Dos16ExternalOracleValidator.Compare(
            operandRegion, missingFactsGraph, operandManifest);
        Equal(unavailable.NotObserved, 1,
            "old graph rows lacking structured facts cannot masquerade as a match");
        Equal(unavailable.Findings[0].DetailCode,
            "candidate-structured-operand-facts-unavailable",
            "missing semantic evidence has explicit uncertainty status");
        var forgedFactsGraph = operandGraph with {
            Instructions = operandGraph.Instructions.Select(i =>
                i.Address.Offset == 0x100
                    ? i with { DecoderFacts = MovAx(0x1235) } : i).ToArray()
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            operandRegion, forgedFactsGraph, operandManifest).Conflicting, 1,
            "forged same-source semantic record is not a verified decode");
        Denied(() => Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, operandManifest with {
                ExpectedInstructions = new[] {
                    operandManifest.ExpectedInstructions[0] with {
                        ExpectedFacts = new Dos16InstructionFacts(
                            "123", 2, new Dos16OperandFact("Register", "AX"),
                            new Dos16OperandFact("Immediate16", ImmediateValue: 0x1234))
                    }
                }
            }), "numeric mnemonic alias must not enter external oracle record");
        Denied(() => Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, operandManifest with {
                ExpectedInstructions = new[] {
                    operandManifest.ExpectedInstructions[0] with {
                        ExpectedFacts = MovAx(0x1234) with {
                            Operand0 = new Dos16OperandFact("Register", "AX",
                                ImmediateValue: 4)
                        }
                    }
                }
            }), "register operand cannot also assert an immediate value");

        var wrongStored = manifest with {
            ExpectedInstructions = new[] {
                manifest.ExpectedInstructions[0] with { OriginalHex = "EB03" }
            }
        };
        var changedBytes = Dos16ExternalOracleValidator.Compare(region, graph, wrongStored);
        Equal(changedBytes.Conflicting, 1,
            "wrong oracle bytes are compared against original, not just CFG row");
        Equal(changedBytes.Findings[0].DetailCode,
            "external-oracle-bytes-disagree-with-original-file",
            "wrong reference-original mismatch is explicit");

        var wrongFlow = manifest with {
            ExpectedInstructions = new[] {
                manifest.ExpectedInstructions[1] with {
                    FlowControl = "UnconditionalBranch"
                }
            }
        };
        var incorrectCategory = Dos16ExternalOracleValidator.Compare(region, graph, wrongFlow);
        Equal(incorrectCategory.Conflicting, 1,
            "wrong external flow-category is a contradictory observation");
        Equal(incorrectCategory.Findings[0].DetailCode,
            "candidate-flow-category-disagrees-with-external-oracle",
            "category mismatch has stable evidence reason");

        var wrongTarget = manifest with {
            ExpectedInstructions = new[] {
                manifest.ExpectedInstructions[0] with { DirectTarget = 0x103 }
            }
        };
        var wrongEdge = Dos16ExternalOracleValidator.Compare(region, graph, wrongTarget);
        Equal(wrongEdge.Missing, 1, "independently expected incorrect target is missing");
        Equal(wrongEdge.Findings[0].DetailCode,
            "externally-expected-direct-edge-not-found",
            "control-flow discrepancy is not counted as a matching decode");

        var undiscovered = manifest with {
            ExpectedInstructions = new[] {
                new Dos16ExternalExpectedInstruction(
                    0x102, 2, "90", "Next", "unseeded-nop")
            }
        };
        var omitted = Dos16ExternalOracleValidator.Compare(region, graph, undiscovered);
        Equal(omitted.NotObserved, 1, "nonvisited but original-backed instruction stays unobserved");
        Equal(omitted.UnbenchmarkedCandidates, 2,
            "candidate instructions outside an external sample remain unbenchmarked");

        var capped = Dos16ControlFlowAnalysis.Analyze(region, 0x100, maxInstructions: 1);
        var partial = Dos16ExternalOracleValidator.Compare(region, capped, manifest);
        Equal(partial.Truncated, true, "resource-limited comparison retains truncated marker");
        Equal(partial.NotObserved, 1, "unvisited RET cannot be claimed decoded by truncated run");

        var forgedRows = graph with {
            Instructions = graph.Instructions.Select(i => i.Address.Offset == 0x100
                ? i with { OriginalBytes = "EB03" } : i).ToArray()
        };
        var forged = Dos16ExternalOracleValidator.Compare(region, forgedRows, manifest);
        Equal(forged.Conflicting, 1, "forged same-source-hash CFG instruction is rejected");

        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with { SourceSha256 = new string('0', 64) }),
            "foreign file SHA cannot validate an unrelated binary");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with { RegionFileOffset = 1 }),
            "same SHA but wrong mapped region cannot validate this instruction window");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with { DecoderName = "Iced.Intel" }),
            "embedded decoder is not an independent oracle");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with { DecoderName = "DOSDis" }),
            "application self-output cannot masquerade as an external oracle");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0], manifest.ExpectedInstructions[0]
                }
            }), "duplicate expectations cannot inflate the denominator");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0],
                    new Dos16ExternalExpectedInstruction(
                        0x101, 1, "02", "Next", "overlaps-short-jump")
                }
            }), "overlapping oracle instructions must not double-count source bytes");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0] with {
                        SourceFileOffset = 100
                    }
                }
            }), "foreign physical file offsets cannot match correct COM segments");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0] with {
                        FlowControl = "123"
                    }
                }
            }), "numeric enum flow aliases are invalid external expectations");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0] with {
                        DirectEdgeKind = "call"
                    }
                }
            }), "external branch kind must match opcode flow category");
        Denied(() => Dos16ExternalOracleValidator.Compare(region, graph,
            manifest with {
                ExpectedInstructions = new[] {
                    manifest.ExpectedInstructions[0] with {
                        DirectTarget = null
                    }
                }
            }), "orphan direct-edge-kind fields are invalid");

        var directory = Path.Combine(Path.GetTempPath(),
            "dosdis-external-oracle-synthetic-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(directory);
        try
        {
            string input = Path.Combine(directory, "oracle.json");
            File.WriteAllText(input, JsonSerializer.Serialize(manifest,
                new JsonSerializerOptions(JsonSerializerDefaults.Web)));
            var roundTrip = Dos16ExternalOracleValidator.ReadManifest(input);
            Equal(roundTrip.SourceSha256, artifact.Sha256,
                "JSON manifest import preserves original SHA-256");
            string output = Path.Combine(directory, "comparison.json");
            Dos16ExternalOracleValidator.WriteNew(output, report);
            using var json = JsonDocument.Parse(File.ReadAllText(output));
            Equal(json.RootElement.GetProperty("independenceStatus").GetString(),
                "claimed-external-not-reviewed",
                "JSON export cannot self-attest reviewer independence");
            bool overwriteDenied = false;
            try { Dos16ExternalOracleValidator.WriteNew(output, report); }
            catch (IOException) { overwriteDenied = true; }
            Equal(overwriteDenied, true,
                "external comparison export does not overwrite prior findings");
            Equal(Directory.EnumerateFiles(directory, "*.part").Any(), false,
                "external comparison staging leaves no partial files");
        }
        finally { Directory.Delete(directory, recursive: true); }

        Console.WriteLine("PASS: source-identified x86-16 external-oracle comparison contract (synthetic only)");
    }
}
