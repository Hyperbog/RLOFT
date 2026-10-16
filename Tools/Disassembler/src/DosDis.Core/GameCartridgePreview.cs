namespace DosDis.Core;

/// <summary>
/// A read-only, source-backed desktop preview for ROMs with a unique static
/// CPU view. It is not a mapper-state inference or a runtime execution trace.
/// </summary>
public sealed record GameCartridgePreviewLine(uint GuestAddress, string OriginalBytes,
    string Assembly, IReadOnlyList<GuestSourceFragment> SourceFragments);

public sealed record GameCartridgePreview(string PlatformLabel, string ViewId,
    IReadOnlyList<GameCartridgePreviewLine> Instructions, int Unresolved,
    bool Truncated, string Qualification)
{
    public static GameCartridgePreview? TryCreate(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        BinaryFormatDetection signature = BinaryFormatDetector.Inspect(artifact);
        try
        {
            if (signature.FormatId == "ines-candidate")
            {
                NesCartridge rom = NesCartridge.Load(artifact);
                // UxROM's lower bank is mutable. Do not pick one and present
                // its speculative code as an automatically validated view.
                if (rom.Mapper != 0) return null;
                var (memory, view) = rom.CreateCpuView();
                var vectors = rom.ReadVectors();
                if (vectors is null || vectors.Value.Reset < 0x8000)
                    return null;
                ushort[] seeds = new[] { vectors.Value.Reset, vectors.Value.Nmi,
                    vectors.Value.Irq }.Where(a => a >= 0x8000).Distinct().ToArray();
                Mos6502Graph graph = Mos6502FlowAnalysis.Analyze(
                    memory, view, seeds, maxInstructions: 256);
                return new GameCartridgePreview("NES NROM / 6502", view,
                    graph.Instructions.Select(x => new GameCartridgePreviewLine(
                        x.Address, x.BytesHex, x.Assembly, x.SourceFragments)).ToArray(),
                    graph.Unresolved.Count, graph.Truncated,
                    "Static source-backed candidate CFG from reset/NMI/IRQ vectors; not observed runtime execution.");
            }

            if (signature.FormatId == "sms-signature-candidate")
            {
                SmsCartridge rom = SmsCartridge.Load(artifact);
                // For a single 16KiB physical bank, both standard ROM mapper
                // profiles expose the same file bytes in every ROM page.
                // Larger cartridges require explicit mapper/slot selection.
                if (rom.Banks != 1) return null;
                var (memory, view) = rom.CreateSegaCpuView(0, 0, 0);
                Z80Graph graph = Z80FlowAnalysis.Analyze(memory, view,
                    new ushort[] { 0 }, maxInstructions: 256);
                return new GameCartridgePreview("SMS / Game Gear Z80, 16KiB ROM",
                    view, graph.Instructions.Select(x => new GameCartridgePreviewLine(
                        x.Address, x.OriginalBytes, x.Assembly, x.SourceFragments)).ToArray(),
                    graph.Unknowns.Count, graph.Truncated,
                    "Single ROM bank, source-backed code candidates from CPU 0000; mapper and execution unverified.");
            }
        }
        catch (InvalidDataException) { /* Preserve unknown original as raw hex. */ }
        catch (NotSupportedException) { /* Unsupported formats remain raw hex. */ }
        return null;
    }
}
