using DosDis.Core;

internal static class Dos16AbstractValueSmoke
{
    private static void Check(bool condition, string name)
    {
        if (!condition) throw new Exception("x86-16 value domain: " + name);
    }

    internal static void Run()
    {
        var bottom = Dos16AbstractValue.Unreachable;
        var top = Dos16AbstractValue.Unknown;
        var one = Dos16AbstractValue.Constant(1);
        var many = Dos16AbstractValue.Finite(4, 1, 4, 2);
        Check(many.Kind == Dos16ValueKind.FiniteSet, "finite-set shape");
        Check(many.Members.SequenceEqual(new ushort[] { 1, 2, 4 }),
            "finite members sorted and unique");

        var input = new ushort[] { 3, 9 };
        var captured = Dos16AbstractValue.Finite(input);
        input[0] = 100;
        Check(captured.Members.SequenceEqual(new ushort[] { 3, 9 }),
            "caller cannot mutate captured values");
        Check(bottom.Covers(bottom) && !bottom.Covers(one) &&
            top.Covers(one) && !one.Covers(top), "top and bottom ordering");
        Check(Dos16AbstractValue.Join(bottom, one).Kind == Dos16ValueKind.Constant,
            "bottom join identity");
        Check(Dos16AbstractValue.Join(top, one).Kind == Dos16ValueKind.Unknown,
            "unknown join absorbs other values");
        Check(Dos16AbstractValue.Join(many, Dos16AbstractValue.Constant(5))
            .Members.SequenceEqual(new ushort[] { 1, 2, 4, 5 }),
            "small exact join");
        var nine = Dos16AbstractValue.Join(
            Dos16AbstractValue.Finite(0, 1, 2, 3, 4, 5, 6, 7),
            Dos16AbstractValue.Constant(9));
        Check(nine.Kind == Dos16ValueKind.UnsignedInterval &&
            nine.Minimum == 0 && nine.Maximum == 9,
            "finite overflow widens to sound unsigned hull");
        Check(Dos16AbstractValue.Interval(1, 9).Covers(many),
            "interval contains sparse exact members");
        Check(!many.Covers(Dos16AbstractValue.Interval(1, 4)),
            "sparse set cannot cover entire interval");

        Check(Dos16AbstractValue.Add(
                Dos16AbstractValue.Finite(0, ushort.MaxValue), one)
            .Members.SequenceEqual(new ushort[] { 0, 1 }),
            "discrete modular carry remains exact");
        var wrap = Dos16AbstractValue.Add(
            Dos16AbstractValue.Interval(65530, 65535),
            Dos16AbstractValue.Constant(10));
        Check(wrap.Minimum == 4 && wrap.Maximum == 9,
            "whole-interval modular carry remains bounded");
        Check(Dos16AbstractValue.Add(
                Dos16AbstractValue.Interval(65530, 65535),
                Dos16AbstractValue.Interval(1, 10)).Kind == Dos16ValueKind.Unknown,
            "mixed-carry interval becomes unknown");
        var negative = Dos16AbstractValue.Subtract(
            Dos16AbstractValue.Interval(0, 5),
            Dos16AbstractValue.Constant(9));
        Check(negative.Minimum == 65527 && negative.Maximum == 65532,
            "uniform subtraction borrow is bounded");
        Check(Dos16AbstractValue.Subtract(
                Dos16AbstractValue.Interval(0, 5),
                Dos16AbstractValue.Constant(2)).Kind == Dos16ValueKind.Unknown,
            "mixed-borrow subtraction is unknown");
        Check(Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Finite(1, 3), 1).Members
                .SequenceEqual(new ushort[] { 2, 6 }),
            "correlated self-doubling preserves exact finite outcomes");
        Check(Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Finite(0x8000, 0xFFFF), 1).Members
                .SequenceEqual(new ushort[] { 0, 0xFFFE }),
            "discrete modular word shift keeps wrapped outcomes");
        Check(Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Interval(2, 5), 2).Minimum == 8 &&
            Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Interval(2, 5), 2).Maximum == 20,
            "nonwrapping shifted interval stays bounded");
        Check(Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Interval(0x3FFE, 0x4001), 2).Kind ==
                Dos16ValueKind.Unknown,
            "mixed modulo quotients invalidate shifted interval");
        Check(Dos16AbstractValue.ShiftLeft(bottom, 1).Kind ==
            Dos16ValueKind.Unreachable &&
            Dos16AbstractValue.ShiftLeft(top, 2).Kind == Dos16ValueKind.Unknown,
            "shift preserves bottom and top");
        bool invalidShift = false;
        try { Dos16AbstractValue.ShiftLeft(one, 16); }
        catch (ArgumentOutOfRangeException) { invalidShift = true; }
        Check(invalidShift, "out-of-domain shift count refused");

        Check(Dos16AbstractValue.Add(bottom, top).Kind == Dos16ValueKind.Unreachable,
            "unreachable arithmetic stays unreachable");
        Check(many.RestrictUnsigned(5, 10).Kind == Dos16ValueKind.Unreachable,
            "empty unsigned guard is unreachable");
        Check(top.RestrictUnsigned(5, 10).Minimum == 5 &&
            top.RestrictUnsigned(5, 10).Maximum == 10,
            "unsigned restriction of unknown creates interval");
        Check(Dos16AbstractValue.Widen(
            Dos16AbstractValue.Interval(0, 5),
            Dos16AbstractValue.Constant(6)).Kind == Dos16ValueKind.Unknown,
            "growth widens rather than falsely converging");

        bool rejectedOversize = false;
        try { Dos16AbstractValue.Finite(0, 1, 2, 3, 4, 5, 6, 7, 8); }
        catch (ArgumentOutOfRangeException) { rejectedOversize = true; }
        Check(rejectedOversize, "public finite-set bound");

        string hash = new('a', 64);
        var start = Dos16ValueOrigin.Create(hash, 100,
            new Dos16Address(0, 0x100), Dos16ValueOriginKind.Immediate);
        var fact = Dos16ValueFact.Observe(one, start);
        for (int i = 1; i <= 9; i++)
        {
            var next = Dos16ValueOrigin.Create(hash, 100 + i,
                new Dos16Address(0, (ushort)(0x100 + i)),
                Dos16ValueOriginKind.UnsignedGuard);
            fact = Dos16ValueFact.Join(fact,
                Dos16ValueFact.Observe(one, next));
        }
        Check(fact.Origins.Count == Dos16ValueFact.MaximumOrigins &&
            fact.OriginsTruncated, "provenance overflow is visible");
        var duplicate = Dos16ValueFact.Join(fact,
            Dos16ValueFact.Observe(one, start));
        Check(duplicate.Origins.Count == Dos16ValueFact.MaximumOrigins,
            "origin labels remain distinct and bounded");
        bool rejectedCrossSource = false;
        try
        {
            Dos16ValueFact.Join(fact, Dos16ValueFact.Observe(one,
                Dos16ValueOrigin.Create(new string('b', 64), 100,
                    new Dos16Address(0, 0x100), Dos16ValueOriginKind.Immediate)));
        }
        catch (InvalidDataException) { rejectedCrossSource = true; }
        Check(rejectedCrossSource, "source hashes may not be merged");

        Console.WriteLine("PASS: x86-16 value lattice, safe modular arithmetic and bounded provenance");
    }
}
