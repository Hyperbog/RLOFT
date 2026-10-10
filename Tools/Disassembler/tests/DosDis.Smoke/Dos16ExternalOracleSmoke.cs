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
        Equal(report.MappedRegionSha256, artifact.Sha256,
            "COM region is the entire original file in this fixture");
        Equal(report.BenchmarkedInstructionBytesSha256,
            "cc662e9a5057407053a11ed44a6974cb8b97b3e85b1ca3f6ba20df75d986e01d",
            "fixed independent SHA of ordered IP/length/original-byte samples");

        // Distinct full original file identities can have the exact same
        // benchmarked instructions: only skipped/unbenchmarked bytes differ.
        // They must not be counted as two independent source observations.
        var changedGap = (byte[])bytes.Clone();
        changedGap[2] = 0xCC;
        changedGap[3] = 0xCC;
        var changedArtifact = BinaryArtifact.FromBytes(changedGap,
            "same-jmp-ret-different-unbenchmarked-gap.com");
        var changedRegion = DosComImage.LoadExplicit(changedArtifact).CreateCodeRegion();
        var changedGraph = Dos16ControlFlowAnalysis.Analyze(changedRegion, 0x100);
        var changedManifest = manifest with {
            SourceSha256 = changedArtifact.Sha256
        };
        var changedReport = Dos16ExternalOracleValidator.Compare(
            changedRegion, changedGraph, changedManifest);
        Equal(changedReport.Matched, 2,
            "same observed sample retains matching original instructions");
        Equal(changedReport.SourceSha256 == report.SourceSha256, false,
            "different whole-file bytes produce distinct source identity");
        Equal(changedReport.MappedRegionSha256 == report.MappedRegionSha256, false,
            "changed unmapped-for-decoding source gap changes mapped-region digest");
        Equal(changedReport.BenchmarkedInstructionBytesSha256,
            report.BenchmarkedInstructionBytesSha256,
            "identical original sampled bytes retain one shared benchmark identity");

        // Two different MZ containers can carry literally the SAME entire
        // source-backed resident image. Their extra trailing overlay
        // bytes alter whole-file hashes but MUST NOT manufacture two
        // independent decoded-code samples or two mapped-region digests.
        var mzResidentWithOverlayA = new byte[37];
        mzResidentWithOverlayA[0] = (byte)'M';
        mzResidentWithOverlayA[1] = (byte)'Z';
        mzResidentWithOverlayA[2] = 36; // one declared 36-byte file page
        mzResidentWithOverlayA[4] = 1;
        mzResidentWithOverlayA[8] = 2;  // 32-byte MZ header
        mzResidentWithOverlayA[32] = 0xEB;
        mzResidentWithOverlayA[33] = 0x00;
        mzResidentWithOverlayA[34] = 0xC3;
        mzResidentWithOverlayA[35] = 0x90;
        mzResidentWithOverlayA[36] = 0x11; // trailing, not part of MZ load image
        var mzResidentWithOverlayB = (byte[])mzResidentWithOverlayA.Clone();
        mzResidentWithOverlayB[36] = 0x22;
        var mzSourceA = BinaryArtifact.FromBytes(mzResidentWithOverlayA,
            "synthetic-shared-mz-with-overlay-A.exe");
        var mzSourceB = BinaryArtifact.FromBytes(mzResidentWithOverlayB,
            "synthetic-shared-mz-with-overlay-B.exe");
        var mzRegionA = DosMzImage.Load(mzSourceA).CreateEntryCodeRegion();
        var mzRegionB = DosMzImage.Load(mzSourceB).CreateEntryCodeRegion();
        var mzManifestA = manifest with {
            SourceSha256 = mzSourceA.Sha256,
            SourceByteLength = mzSourceA.Length,
            RegionFileOffset = mzRegionA.FileOffset,
            RegionByteLength = mzRegionA.ByteLength,
            Segment = mzRegionA.Segment,
            Origin = mzRegionA.Origin,
            ExpectedInstructions = new[] {
                new Dos16ExternalExpectedInstruction(0, 32, "EB00",
                    "UnconditionalBranch", "source-backed-outer-mz-short-jmp",
                    "branch", 2),
                new Dos16ExternalExpectedInstruction(2, 34, "C3",
                    "Return", "source-backed-outer-mz-ret")
            }
        };
        var mzReportA = Dos16ExternalOracleValidator.Compare(
            mzRegionA, Dos16ControlFlowAnalysis.Analyze(mzRegionA, 0),
            mzManifestA);
        var mzReportB = Dos16ExternalOracleValidator.Compare(
            mzRegionB, Dos16ControlFlowAnalysis.Analyze(mzRegionB, 0),
            mzManifestA with { SourceSha256 = mzSourceB.Sha256 });
        Equal(mzReportA.Matched, 2,
            "first synthetic MZ declared launcher rows remain source backed");
        Equal(mzReportB.Matched, 2,
            "second MZ has same resident code despite different appended data");
        Equal(mzReportA.SourceSha256 == mzReportB.SourceSha256, false,
            "distinct outer MZ containers retain distinct full-file hashes");
        Equal(mzReportA.MappedRegionSha256,
            mzReportB.MappedRegionSha256,
            "identical MZ mapped resident bytes share one source window identity");
        Equal(mzReportA.BenchmarkedInstructionBytesSha256,
            mzReportB.BenchmarkedInstructionBytesSha256,
            "identical MZ instruction samples cannot inflate independent coverage");
        Equal(DosMzImage.Load(mzSourceA).HasTrailingOverlayData, true,
            "synthetic source difference remains outside declared MZ module");

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
        Equal(operandMatch.StructuredOperandExpectations, 3,
            "structured operand denominator excludes RET with no operand assertion");
        Equal(operandMatch.FullyMatchedOperandExpectations, 3,
            "three explicit register/immediate semantic assertions agree");
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
        Equal(immediateMismatch.StructuredOperandExpectations, 1,
            "single structured expectation retains its own denominator");
        Equal(immediateMismatch.FullyMatchedOperandExpectations, 0,
            "wrong immediate never scores a fully matched semantic expectation");
        Equal(immediateMismatch.Findings[0].DetailCode,
            "candidate-structured-operands-disagree-with-external-oracle",
            "operand-value disagreement has a machine-readable finding");
        var wrongSignedValue = operandManifest with {
            ExpectedInstructions = new[] {
                operandManifest.ExpectedInstructions[2] with {
                    ExpectedFacts = CmpBxSigned() with {
                        Operand1 = new Dos16OperandFact(
                            "Immediate8to16", ImmediateValue: -1)
                    }
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, wrongSignedValue).Conflicting, 1,
            "validly encoded but incorrect signed immediate is a semantic conflict");
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
        // A forged cached decoder fact can agree with an equally wrong
        // manifest, but cannot change what the source bytes actually decode.
        var jointlyWrongFacts = Dos16ExternalOracleValidator.Compare(
            operandRegion, forgedFactsGraph, wrongImmediate);
        Equal(jointlyWrongFacts.Conflicting, 1,
            "two agreeing false operand records cannot validate original bytes");
        Equal(jointlyWrongFacts.Findings[0].DetailCode,
            "candidate-decoder-or-direct-edge-disagrees-with-original-bytes",
            "cached semantic agreement must be independently re-decoded from source");
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

        var wrongMnemonic = operandManifest with {
            ExpectedInstructions = new[] {
                operandManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MovAx(0x1234) with { Mnemonic = "Add" }
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, wrongMnemonic).Conflicting, 1,
            "matching original bytes and registers do not excuse wrong mnemonic");
        Denied(() => Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, operandManifest with {
                ExpectedInstructions = new[] {
                    operandManifest.ExpectedInstructions[0] with {
                        ExpectedFacts = MovAx(0x1234) with {
                            OperandCount = 3
                        }
                    }
                }
            }), "three-operand expectations exceed explicit supported scope");
        Denied(() => Dos16ExternalOracleValidator.Compare(
            operandRegion, operandGraph, operandManifest with {
                ExpectedInstructions = new[] {
                    operandManifest.ExpectedInstructions[0] with {
                        ExpectedFacts = MovAx(0x1234) with {
                            Operand0 = new Dos16OperandFact("Memory")
                        }
                    }
                }
            }), "unmodeled memory address semantics cannot pass narrow oracle");

        // 16-bit ModR/M memory address encoding oracle:
        // MOV AX,[BX+SI-2]; MOV AX,DS:[BP+0] (explicit override);
        // MOV AX,[1234h]; MOV AL,[BX]; MOV AX,[EAX] (67h override);
        // RET. The 32-bit addressing override deliberately stays
        // outside this exact 16-bit memory semantic subset.
        byte[] memoryBytes = {
            0x8B, 0x40, 0xFE,
            0x3E, 0x8B, 0x46, 0x00,
            0x8B, 0x06, 0x34, 0x12,
            0x8A, 0x07,
            0x67, 0x8B, 0x00,
            0xC3
        };
        var memoryArtifact = BinaryArtifact.FromBytes(memoryBytes,
            "synthetic-memory-oracle.com");
        var memoryRegion = DosComImage.LoadExplicit(memoryArtifact).CreateCodeRegion();
        var memoryGraph = Dos16ControlFlowAnalysis.Analyze(memoryRegion, 0x100);
        Dos16InstructionFacts MemoryMov(string register,
            string? baseReg, string? indexReg, int displacementSize,
            uint displacement, string size, string? segment = null) =>
            new("Mov", 2, new Dos16OperandFact("Register", register),
                new Dos16OperandFact("Memory", Memory:
                    new Dos16MemoryAddressFact(baseReg, indexReg,
                        displacementSize, displacement, size, segment)));
        var memoryManifest = manifest with {
            SourceSha256 = memoryArtifact.Sha256,
            SourceByteLength = memoryArtifact.Length,
            RegionByteLength = memoryRegion.ByteLength,
            ExpectedInstructions = new[] {
                new Dos16ExternalExpectedInstruction(
                    0x100, 0, "8B40FE", "Next", "base-index-negative-disp8",
                    ExpectedFacts: MemoryMov("AX", "BX", "SI", 1, 0xFFFE, "UInt16")),
                new Dos16ExternalExpectedInstruction(
                    0x103, 3, "3E8B4600", "Next", "bp-explicit-ds-override",
                    ExpectedFacts: MemoryMov("AX", "BP", null, 1, 0, "UInt16", "DS")),
                new Dos16ExternalExpectedInstruction(
                    0x107, 7, "8B063412", "Next", "direct-address-disp16",
                    ExpectedFacts: MemoryMov("AX", null, null, 2, 0x1234, "UInt16")),
                new Dos16ExternalExpectedInstruction(
                    0x10B, 11, "8A07", "Next", "byte-memory-width",
                    ExpectedFacts: MemoryMov("AL", "BX", null, 0, 0, "UInt8")),
                new Dos16ExternalExpectedInstruction(
                    0x10D, 13, "678B00", "Next", "32bit-address-override-unknown"),
                new Dos16ExternalExpectedInstruction(
                    0x110, 16, "C3", "Return", "memory-fixture-return")
            }
        };
        var memoryReport = Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, memoryManifest);
        Equal(memoryReport.Matched, 6,
            "independent memory-format candidates preserve expected source boundaries");
        Equal(memoryReport.StructuredOperandExpectations, 4,
            "memory form benchmark denominator excludes 67h unsupported addressing");
        Equal(memoryReport.FullyMatchedOperandExpectations, 4,
            "four explicit original 16-bit scalar memory forms match");
        Equal(memoryGraph.Instructions.Single(i =>
            i.Address.Offset == 0x10D).DecoderFacts!.Operand1!.Memory is null,
            true, "32-bit address-size override is not an accepted 16-bit addressing fact");

        // Exact byte-width boundaries: signed disp8 -128 must map
        // to FF80 in a 16-bit address, and +127 to 007F.
        // Do not infer either from a formatter's signed/unsigned text.
        var displacementBoundaryBytes = new byte[] {
            0x8B, 0x40, 0x80, 0x8B, 0x40, 0x7F, 0xC3
        };
        var boundaryArtifact = BinaryArtifact.FromBytes(
            displacementBoundaryBytes, "synthetic-disp8-boundaries.com");
        var boundaryRegion = DosComImage.LoadExplicit(boundaryArtifact).CreateCodeRegion();
        var boundaryGraph = Dos16ControlFlowAnalysis.Analyze(boundaryRegion, 0x100);
        var boundaryManifest = manifest with {
            SourceSha256 = boundaryArtifact.Sha256,
            SourceByteLength = boundaryArtifact.Length,
            RegionByteLength = boundaryRegion.ByteLength,
            ExpectedInstructions = new[] {
                new Dos16ExternalExpectedInstruction(
                    0x100, 0, "8B4080", "Next", "negative-128-disp8",
                    ExpectedFacts: MemoryMov("AX", "BX", "SI", 1,
                        0xFF80, "UInt16")),
                new Dos16ExternalExpectedInstruction(
                    0x103, 3, "8B407F", "Next", "positive-127-disp8",
                    ExpectedFacts: MemoryMov("AX", "BX", "SI", 1,
                        0x007F, "UInt16")),
                new Dos16ExternalExpectedInstruction(
                    0x106, 6, "C3", "Return", "boundary-fixture-return")
            }
        };
        var boundaryReport = Dos16ExternalOracleValidator.Compare(
            boundaryRegion, boundaryGraph, boundaryManifest);
        Equal(boundaryReport.Matched, 3,
            "both valid signed disp8 extremes remain source-backed matches");
        Equal(boundaryReport.FullyMatchedOperandExpectations, 2,
            "two disp8 encoding-boundary oracle facts must agree");
        Equal(boundaryGraph.Instructions.Single(i =>
            i.Address.Offset == 0x100).DecoderFacts!.Operand1!.Memory!.Displacement,
            0xFF80u, "negative 128 disp8 sign-extends modulo 16 bits");
        Equal(boundaryGraph.Instructions.Single(i =>
            i.Address.Offset == 0x103).DecoderFacts!.Operand1!.Memory!.Displacement,
            0x007Fu, "positive 127 disp8 remains positive modulo 16 bits");

        var wrongMemoryRegister = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MemoryMov("AX", "BP", "SI", 1, 0xFFFE, "UInt16")
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, wrongMemoryRegister).Conflicting, 1,
            "different memory base register fails comparison with identical bytes");

        var wrongMemoryDisplacement = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MemoryMov("AX", "BX", "SI", 1, 0xFFFD, "UInt16")
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, wrongMemoryDisplacement).Conflicting, 1,
            "a valid but incorrect negative disp8 cannot pass");

        var wrongDisplacementWidth = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[0] with {
                    ExpectedFacts = MemoryMov("AX", "BX", "SI", 2, 0xFFFE, "UInt16")
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, wrongDisplacementWidth).Conflicting, 1,
            "disp8 and disp16 encodings are independently distinguishable");

        var wrongSegmentOverride = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[1] with {
                    ExpectedFacts = MemoryMov("AX", "BP", null, 1, 0, "UInt16")
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, wrongSegmentOverride).Conflicting, 1,
            "explicit DS prefix is not interchangeable with default BP/SS");

        var wrongMemorySize = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[3] with {
                    ExpectedFacts = MemoryMov("AL", "BX", null, 0, 0, "UInt16")
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, wrongMemorySize).Conflicting, 1,
            "byte memory access cannot be reclassified as a 16-bit access");

        var unsupportedOverride = memoryManifest with {
            ExpectedInstructions = new[] {
                memoryManifest.ExpectedInstructions[4] with {
                    ExpectedFacts = MemoryMov("AX", "BX", null, 0, 0, "UInt16")
                }
            }
        };
        var unresolvedOverride = Dos16ExternalOracleValidator.Compare(
            memoryRegion, memoryGraph, unsupportedOverride);
        Equal(unresolvedOverride.NotObserved, 1,
            "unsupported address-size override cannot claim 16-bit memory matching");
        Equal(unresolvedOverride.Findings[0].DetailCode,
            "candidate-memory-address-facts-outside-supported-scope",
            "unsupported address size remains explicit unobserved evidence");

        var noMemoryGraph = memoryGraph with {
            Instructions = memoryGraph.Instructions.Select(i =>
                i.Address.Offset == 0x100
                    ? i with {
                        DecoderFacts = i.DecoderFacts! with {
                            Operand1 = new Dos16OperandFact("Memory")
                        }
                    } : i).ToArray()
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            memoryRegion, noMemoryGraph, memoryManifest).NotObserved, 1,
            "legacy graph memory operand without address facts is unobserved");

        void RejectMemory(Dos16MemoryAddressFact m, string why)
        {
            var invalid = memoryManifest with {
                ExpectedInstructions = new[] {
                    memoryManifest.ExpectedInstructions[0] with {
                        ExpectedFacts = MemoryMov("AX", m.Base, m.Index,
                            m.DisplacementSize, m.Displacement, m.Size,
                            m.SegmentOverride)
                    }
                }
            };
            Denied(() => Dos16ExternalOracleValidator.Compare(
                memoryRegion, memoryGraph, invalid), why);
        }
        RejectMemory(new("BX", "DI", 1, 0x1234, "UInt16", null),
            "invalid disp8 modulo16 representation cannot enter oracle");
        RejectMemory(new(null, null, 0, 0, "UInt16", null),
            "direct memory form must encode disp16");
        RejectMemory(new("BP", null, 0, 0, "UInt16", null),
            "bare BP cannot use mod=00 without an encoded displacement");
        RejectMemory(new("SI", "DI", 1, 2, "UInt16", null),
            "invalid 16-bit SI+DI register pairing cannot enter oracle");
        RejectMemory(new("EAX", null, 0, 0, "UInt16", null),
            "32-bit base register cannot be called a 16-bit address");
        RejectMemory(new("BX", null, 0, 0, "Fword6", null),
            "unsupported far-pointer width cannot be asserted");
        RejectMemory(new("BX", null, 0, 0, "UInt16", "FSX"),
            "invented segment override cannot be asserted");

        var wrongStored = manifest with {
            ExpectedInstructions = new[] {
                manifest.ExpectedInstructions[0] with { OriginalHex = "EB03" }
            }
        };
        var changedBytes = Dos16ExternalOracleValidator.Compare(region, graph, wrongStored);
        Equal(changedBytes.Conflicting, 1,
            "wrong oracle bytes are compared against original, not just CFG row");
        Equal(changedBytes.BenchmarkedInstructionBytesSha256,
            report.BenchmarkedInstructionBytesSha256,
            "sample identity hashes actual original bytes, never forged oracle hex");
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

        // A fabricated graph edge can make a wrong outside target look
        // present. Even then, its decoded direct target must agree with
        // the unchanged original EB 02 instruction.
        var forgedEdgeGraph = graph with {
            Edges = graph.Edges.Concat(new[] {
                new Dos16FlowEdge(new Dos16Address(region.Segment, 0x100),
                    new Dos16Address(region.Segment, 0x103),
                    "branch", "forged-cached-edge")
            }).ToArray()
        };
        var falsePositiveEdge = Dos16ExternalOracleValidator.Compare(
            region, forgedEdgeGraph, wrongTarget);
        Equal(falsePositiveEdge.Conflicting, 1,
            "cached false direct edge cannot manufacture an oracle match");
        Equal(falsePositiveEdge.Findings[0].DetailCode,
            "candidate-decoder-or-direct-edge-disagrees-with-original-bytes",
            "re-decoded direct target is checked after graph presence");

        // A forged graph flow label plus matching false oracle flow label
        // must also conflict with the original jump opcode.
        var forgedFlowGraph = graph with {
            Instructions = graph.Instructions.Select(i => i.Address.Offset == 0x100
                ? i with { FlowControl = "Return" } : i).ToArray()
        };
        var forgedFlowManifest = manifest with {
            ExpectedInstructions = new[] {
                manifest.ExpectedInstructions[0] with {
                    FlowControl = "Return", DirectEdgeKind = null, DirectTarget = null
                }
            }
        };
        Equal(Dos16ExternalOracleValidator.Compare(
            region, forgedFlowGraph, forgedFlowManifest).Conflicting, 1,
            "same original jump bytes cannot pass as an invented return flow");

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

            // Web-default JSON deserialization is case-insensitive and would
            // otherwise silently select one of two conflicting claims.
            // Reject duplicate names at the root and nested instruction level.
            var serializedManifest = File.ReadAllText(input);
            const string schemaAnchor = "\"schemaVersion\":1,";
            const string hexAnchor = "\"originalHex\":\"EB02\"";
            Equal(serializedManifest.Contains(schemaAnchor, StringComparison.Ordinal),
                true, "fixture exposes an unambiguous source schema key");
            Equal(serializedManifest.Contains(hexAnchor, StringComparison.Ordinal),
                true, "fixture exposes the first original instruction key");
            string duplicateRoot = Path.Combine(directory, "ambiguous-root.json");
            File.WriteAllText(duplicateRoot, serializedManifest.Replace(
                schemaAnchor, "\"schemaVersion\":1,\"SCHEMAVERSION\":1,",
                StringComparison.Ordinal));
            Denied(() => Dos16ExternalOracleValidator.ReadManifest(duplicateRoot),
                "duplicated case-insensitive root identity field must be rejected");
            string duplicateInstruction = Path.Combine(directory, "ambiguous-instruction.json");
            File.WriteAllText(duplicateInstruction, serializedManifest.Replace(
                hexAnchor, "\"originalHex\":\"EB02\",\"ORIGINALHEX\":\"EB03\"",
                StringComparison.Ordinal));
            Denied(() => Dos16ExternalOracleValidator.ReadManifest(duplicateInstruction),
                "conflicting original-byte claims in one nested instruction must be rejected");

            // Unknown names are also prohibited: silently ignoring a typo
            // in an optional edge or operand assertion would reduce the
            // reference denominator without the reviewer's knowledge.
            const string edgeAnchor = "\"directEdgeKind\":\"branch\"";
            Equal(serializedManifest.Contains(edgeAnchor, StringComparison.Ordinal),
                true, "fixture contains a required direct-edge assertion");
            string missingEdgeClaim = Path.Combine(directory, "misspelled-edge.json");
            File.WriteAllText(missingEdgeClaim, serializedManifest.Replace(
                edgeAnchor, "\"directEdgeKnd\":\"branch\"",
                StringComparison.Ordinal));
            Denied(() => Dos16ExternalOracleValidator.ReadManifest(missingEdgeClaim),
                "misspelled optional expected direct-edge kind cannot be silently ignored");

            string semanticJson = JsonSerializer.Serialize(operandManifest,
                new JsonSerializerOptions(JsonSerializerDefaults.Web));
            const string factsAnchor = "\"expectedFacts\":{";
            Equal(semanticJson.Contains(factsAnchor, StringComparison.Ordinal),
                true, "fixture contains a structured operand expectation");
            string lostFacts = Path.Combine(directory, "misspelled-facts.json");
            File.WriteAllText(lostFacts, semanticJson.Replace(
                factsAnchor, "\"expectedFakts\":{", StringComparison.Ordinal));
            Denied(() => Dos16ExternalOracleValidator.ReadManifest(lostFacts),
                "misspelled expectedFacts cannot silently remove semantic assertions");

            string memoryJson = JsonSerializer.Serialize(memoryManifest,
                new JsonSerializerOptions(JsonSerializerDefaults.Web));
            const string memoryAnchor = "\"displacementSize\":1";
            Equal(memoryJson.Contains(memoryAnchor, StringComparison.Ordinal),
                true, "fixture includes a source-encoded memory displacement width");
            string lostMemory = Path.Combine(directory, "misspelled-memory.json");
            File.WriteAllText(lostMemory, memoryJson.Replace(
                memoryAnchor, "\"displacmentSize\":1", StringComparison.Ordinal));
            Denied(() => Dos16ExternalOracleValidator.ReadManifest(lostMemory),
                "misspelled optional memory width cannot be silently ignored");
            string output = Path.Combine(directory, "comparison.json");
            Dos16ExternalOracleValidator.WriteNew(output, report);
            using var json = JsonDocument.Parse(File.ReadAllText(output));
            Equal(json.RootElement.GetProperty("independenceStatus").GetString(),
                "claimed-external-not-reviewed",
                "JSON export cannot self-attest reviewer independence");
            Equal(json.RootElement.GetProperty("mappedRegionSha256").GetString(),
                report.MappedRegionSha256,
                "export includes exact original mapped-region digest");
            Equal(json.RootElement.GetProperty("benchmarkedInstructionBytesSha256").GetString(),
                report.BenchmarkedInstructionBytesSha256,
                "export includes source-backed sampled-instruction digest");
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
