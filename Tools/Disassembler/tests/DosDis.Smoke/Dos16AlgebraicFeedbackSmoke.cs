using DosDis.Core;

/// <summary>
/// Synthetic cross-product arithmetic/abstract-lattice feedback.
/// Concrete reference values come from a bounded independent enumerator.
/// No game files, host services or background test execution are involved.
/// </summary>
internal static class Dos16AlgebraicFeedbackSmoke
{
    private const uint Seed = 0xA16E2026;
    private static readonly ushort[] Corners =
    {
        0, 1, 2, 7, 8, 15, 0x00FF, 0x0100, 0x7FFF, 0x8000,
        0xFFFE, 0xFFFF
    };

    private static uint Next(ref uint state)
    {
        uint x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return state = x;
    }

    private static ushort Sample(ref uint seed) =>
        (Next(ref seed) & 3) == 0
            ? Corners[(int)(Next(ref seed) % (uint)Corners.Length)]
            : (ushort)Next(ref seed);

    private static (Dos16AbstractValue Domain, ushort[] Concrete)
        Generate(ref uint seed, int variant)
    {
        ushort start = Sample(ref seed);
        if (variant % 3 == 0)
            return (Dos16AbstractValue.Constant(start), new[] { start });
        if (variant % 3 == 1)
        {
            var elements = Enumerable.Range(0, 1 + (int)(Next(ref seed) % 6))
                .Select(_ => Sample(ref seed)).Distinct().OrderBy(x => x)
                .ToArray();
            return (Dos16AbstractValue.Finite(elements), elements);
        }

        int end = Math.Min(ushort.MaxValue,
            (int)start + (int)(Next(ref seed) % 13));
        var concrete = Enumerable.Range(start, end - start + 1)
            .Select(x => (ushort)x).ToArray();
        return (Dos16AbstractValue.Interval(start, (ushort)end), concrete);
    }

    private static bool Contains(Dos16AbstractValue value, ushort member) =>
        value.Kind switch
        {
            Dos16ValueKind.Unknown => true,
            Dos16ValueKind.Constant or Dos16ValueKind.FiniteSet =>
                value.Members.Contains(member),
            Dos16ValueKind.UnsignedInterval =>
                member >= value.Minimum && member <= value.Maximum,
            _ => false
        };

    private static void Check(bool value, string family, int id, string detail)
    {
        if (!value)
            throw new Exception("x86-16 algebraic feedback seed=0x" +
                Seed.ToString("X8") + " family=" + family +
                " case=" + id + " " + detail);
    }

    internal static void Run()
    {
        uint state = Seed;
        const int cases = 256;
        int concreteOutcomes = 0;
        int rangeRestrictions = 0;

        for (int i = 0; i < cases; i++)
        {
            var (left, leftValues) = Generate(ref state, i);
            var (right, rightValues) = Generate(ref state, i / 3 + 1);

            // Mathematical set union is a concrete independent oracle.
            // A widened hull may include extra values, never omit inputs.
            var joined = Dos16AbstractValue.Join(left, right);
            var reverse = Dos16AbstractValue.Join(right, left);
            foreach (ushort value in leftValues.Concat(rightValues))
                Check(Contains(joined, value) && Contains(reverse, value),
                    "join-soundness", i, "value=" + value +
                    " left=" + left.Kind + " right=" + right.Kind);
            Check(joined.Covers(left) && joined.Covers(right) &&
                reverse.Covers(left) && reverse.Covers(right),
                "join-coverage", i,
                "left=" + left.Kind + " right=" + right.Kind +
                " joined=" + joined.Kind + " reverse=" + reverse.Kind);

            // Widening must never drop an existing predecessor state.
            var widened = Dos16AbstractValue.Widen(left, joined);
            foreach (ushort value in leftValues.Concat(rightValues))
                Check(Contains(widened, value),
                    "widen-soundness", i, "value=" + value +
                    " result=" + widened.Kind);

            // The CPU ALU is modular, independently evaluated for each
            // bounded concrete pair, not implemented using AbstractValue.
            var added = Dos16AbstractValue.Add(left, right);
            var subtracted = Dos16AbstractValue.Subtract(left, right);
            foreach (ushort a in leftValues)
                foreach (ushort b in rightValues)
                {
                    ushort sum = unchecked((ushort)(a + b));
                    ushort difference = unchecked((ushort)(a - b));
                    Check(Contains(added, sum) &&
                        Contains(subtracted, difference),
                        "modular-arithmetic", i,
                        "a=0x" + a.ToString("X4") +
                        " b=0x" + b.ToString("X4") +
                        " add=0x" + sum.ToString("X4") +
                        " sub=0x" + difference.ToString("X4") +
                        " abstractions=" + added.Kind + "/" + subtracted.Kind);
                    concreteOutcomes++;
                }

            // Unsigned restriction is exact for generated finite sets and
            // contiguous intervals. Test *both* excluded and included
            // concrete possibilities rather than only endpoints.
            ushort lower = Sample(ref state);
            ushort upper = (ushort)Math.Min(ushort.MaxValue,
                (int)lower + (int)(Next(ref state) % 33));
            var narrowed = left.RestrictUnsigned(lower, upper);
            var selected = leftValues.Where(x => x >= lower && x <= upper)
                .ToArray();
            Check((narrowed.Kind == Dos16ValueKind.Unreachable) ==
                (selected.Length == 0),
                "unsigned-range-empty", i,
                "lower=" + lower + " upper=" + upper +
                " original=" + left.Kind + " narrowed=" + narrowed.Kind);
            foreach (ushort concrete in leftValues)
                Check(Contains(narrowed, concrete) ==
                    (concrete >= lower && concrete <= upper),
                    "unsigned-range-membership", i,
                    "value=" + concrete + " bounds=" + lower + ":" + upper +
                    " result=" + narrowed.Kind);
            rangeRestrictions++;
        }

        // Explicit alias/extreme arithmetic profiles complement the
        // seeded corpus even if random values happen to avoid boundaries.
        var high = Dos16AbstractValue.Finite(0, 0x7FFF, 0x8000, 0xFFFF);
        var one = Dos16AbstractValue.Constant(1);
        var plus = Dos16AbstractValue.Add(high, one);
        var minus = Dos16AbstractValue.Subtract(high, one);
        Check(plus.Members.SequenceEqual(new ushort[] { 0, 1, 0x8000, 0x8001 }) &&
            minus.Members.SequenceEqual(new ushort[] { 0x7FFE, 0x7FFF, 0xFFFE, 0xFFFF }),
            "extreme-carry-borrow", cases,
            "overflow and underflow must be exact for small concrete sets");

        Console.WriteLine("PASS: bounded x86-16 algebraic feedback" +
            " seed=0x" + Seed.ToString("X8") +
            " domainPairs=" + cases +
            " concreteArithmeticPairs=" + concreteOutcomes +
            " unsignedRestrictions=" + rangeRestrictions);
    }
}
