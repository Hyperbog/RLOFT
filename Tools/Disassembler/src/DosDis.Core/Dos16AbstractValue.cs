namespace DosDis.Core;

/// <summary>
/// Bounded unsigned x86-16 abstract values. No interpretation of source
/// bytes, executable reachability, runtime registers or flags is implied.
/// </summary>
public enum Dos16ValueKind
{
    Unreachable,
    Unknown,
    Constant,
    FiniteSet,
    UnsignedInterval
}

public sealed class Dos16AbstractValue
{
    public const int MaximumFiniteMembers = 8;
    private readonly ushort[] _members;
    private readonly IReadOnlyList<ushort> _view;

    public Dos16ValueKind Kind { get; }
    public ushort Minimum { get; }
    public ushort Maximum { get; }
    public IReadOnlyList<ushort> Members => _view;

    private Dos16AbstractValue(Dos16ValueKind kind, ushort minimum,
        ushort maximum, ushort[]? members = null)
    {
        Kind = kind;
        Minimum = minimum;
        Maximum = maximum;
        _members = members is null ? Array.Empty<ushort>() : (ushort[])members.Clone();
        _view = Array.AsReadOnly(_members);
    }

    public static Dos16AbstractValue Unreachable { get; } =
        new(Dos16ValueKind.Unreachable, 0, 0);
    public static Dos16AbstractValue Unknown { get; } =
        new(Dos16ValueKind.Unknown, 0, ushort.MaxValue);

    public static Dos16AbstractValue Constant(ushort value) =>
        new(Dos16ValueKind.Constant, value, value, new[] { value });

    public static Dos16AbstractValue Interval(ushort minimum, ushort maximum)
    {
        if (minimum > maximum) throw new ArgumentOutOfRangeException(nameof(minimum));
        return minimum == maximum
            ? Constant(minimum)
            : new Dos16AbstractValue(Dos16ValueKind.UnsignedInterval, minimum, maximum);
    }

    public static Dos16AbstractValue Finite(params ushort[] values)
    {
        ArgumentNullException.ThrowIfNull(values);
        if (values.Length is < 1 or > MaximumFiniteMembers)
            throw new ArgumentOutOfRangeException(nameof(values),
                "Finite-set input must contain 1..8 members.");
        return FromExact(values);
    }

    private static Dos16AbstractValue FromExact(IEnumerable<ushort> values)
    {
        // Internal arithmetic has at most 8*8 outcomes; normalize before
        // widening. No unbounded caller enumerable reaches this method.
        ushort[] ordered = values.Distinct().OrderBy(x => x).ToArray();
        if (ordered.Length == 0) return Unreachable;
        if (ordered.Length == 1) return Constant(ordered[0]);
        return ordered.Length <= MaximumFiniteMembers
            ? new Dos16AbstractValue(Dos16ValueKind.FiniteSet,
                ordered[0], ordered[^1], ordered)
            : Interval(ordered[0], ordered[^1]);
    }

    private bool Discrete =>
        Kind is Dos16ValueKind.Constant or Dos16ValueKind.FiniteSet;

    /// <summary>True only if every possible member of other is covered.</summary>
    public bool Covers(Dos16AbstractValue other)
    {
        ArgumentNullException.ThrowIfNull(other);
        if (other.Kind == Dos16ValueKind.Unreachable) return true;
        if (Kind == Dos16ValueKind.Unknown) return true;
        if (Kind == Dos16ValueKind.Unreachable ||
            other.Kind == Dos16ValueKind.Unknown) return false;
        if (other.Minimum < Minimum || other.Maximum > Maximum) return false;
        if (Kind == Dos16ValueKind.UnsignedInterval) return true;
        if (other.Kind == Dos16ValueKind.UnsignedInterval) return false;
        return other._members.All(_members.Contains);
    }

    /// <summary>Sound union; a large finite union is widened to its hull.</summary>
    public static Dos16AbstractValue Join(Dos16AbstractValue left,
        Dos16AbstractValue right)
    {
        ArgumentNullException.ThrowIfNull(left);
        ArgumentNullException.ThrowIfNull(right);
        if (left.Covers(right)) return left;
        if (right.Covers(left)) return right;
        if (left.Discrete && right.Discrete)
            return FromExact(left._members.Concat(right._members));
        return Interval(Math.Min(left.Minimum, right.Minimum),
            Math.Max(left.Maximum, right.Maximum));
    }

    /// <summary>
    /// Loop widening: an expanding value becomes Unknown. Does not assert
    /// convergence, or use speculative instruction/flag semantics.
    /// </summary>
    public static Dos16AbstractValue Widen(Dos16AbstractValue previous,
        Dos16AbstractValue incoming)
    {
        ArgumentNullException.ThrowIfNull(previous);
        ArgumentNullException.ThrowIfNull(incoming);
        if (previous.Covers(incoming)) return previous;
        if (previous.Kind == Dos16ValueKind.Unreachable) return incoming;
        return Unknown;
    }

    public Dos16AbstractValue RestrictUnsigned(ushort minimum, ushort maximum)
    {
        if (minimum > maximum) throw new ArgumentOutOfRangeException(nameof(minimum));
        if (Kind == Dos16ValueKind.Unreachable) return Unreachable;
        if (Kind == Dos16ValueKind.Unknown) return Interval(minimum, maximum);
        if (Maximum < minimum || Minimum > maximum) return Unreachable;
        if (Discrete)
            return FromExact(_members.Where(x => x >= minimum && x <= maximum));
        return Interval(Math.Max(Minimum, minimum), Math.Min(Maximum, maximum));
    }

    public static Dos16AbstractValue Add(Dos16AbstractValue left,
        Dos16AbstractValue right) => Arithmetic(left, right, subtract: false);

    public static Dos16AbstractValue Subtract(Dos16AbstractValue left,
        Dos16AbstractValue right) => Arithmetic(left, right, subtract: true);

    /// <summary>
    /// Exact unary 16-bit modular left-shift for a bounded finite set.
    /// Intervals retain a conservative hull only when every outcome lies
    /// in the same 64-KiB quotient (no modular boundary crossing).
    /// A register doubled from itself is correlated, not a cartesian sum.
    /// </summary>
    public static Dos16AbstractValue ShiftLeft(Dos16AbstractValue value, int count)
    {
        ArgumentNullException.ThrowIfNull(value);
        if (count is < 0 or > 15)
            throw new ArgumentOutOfRangeException(nameof(count));
        if (value.Kind is Dos16ValueKind.Unreachable or Dos16ValueKind.Unknown ||
            count == 0) return value;
        if (value.Discrete)
            return FromExact(value._members.Select(x =>
                unchecked((ushort)(x << count))));
        long minimum = (long)value.Minimum << count;
        long maximum = (long)value.Maximum << count;
        if (minimum / 65536 != maximum / 65536)
            return Unknown;
        return Interval(unchecked((ushort)minimum),
            unchecked((ushort)maximum));
    }

    private static Dos16AbstractValue Arithmetic(Dos16AbstractValue left,
        Dos16AbstractValue right, bool subtract)
    {
        ArgumentNullException.ThrowIfNull(left);
        ArgumentNullException.ThrowIfNull(right);
        if (left.Kind == Dos16ValueKind.Unreachable ||
            right.Kind == Dos16ValueKind.Unreachable) return Unreachable;
        if (left.Kind == Dos16ValueKind.Unknown ||
            right.Kind == Dos16ValueKind.Unknown) return Unknown;

        if (left.Discrete && right.Discrete)
        {
            var results = new List<ushort>(MaximumFiniteMembers * MaximumFiniteMembers);
            foreach (ushort a in left._members)
                foreach (ushort b in right._members)
                    results.Add(unchecked((ushort)(subtract ? a - b : a + b)));
            return FromExact(results);
        }

        // A hull is safe only when all possible results lie entirely on
        // one side of the 16-bit modulo boundary. Otherwise report Unknown.
        int low = subtract
            ? left.Minimum - right.Maximum
            : left.Minimum + right.Minimum;
        int high = subtract
            ? left.Maximum - right.Minimum
            : left.Maximum + right.Maximum;
        if (subtract)
        {
            if (low < 0 && high >= 0) return Unknown;
            int offset = high < 0 ? 65536 : 0;
            return Interval(checked((ushort)(low + offset)),
                checked((ushort)(high + offset)));
        }
        if (low < 65536 && high >= 65536) return Unknown;
        int carry = low >= 65536 ? 65536 : 0;
        return Interval(checked((ushort)(low - carry)),
            checked((ushort)(high - carry)));
    }
}

public enum Dos16ValueOriginKind
{
    Immediate,
    UnsignedGuard,
    RegisterCopy,
    Arithmetic
}

/// <summary>
/// A source-identity label, not proof that the instruction at this address
/// is executable. Decoder-verified bytes must be checked by its caller.
/// </summary>
public sealed class Dos16ValueOrigin
{
    public string SourceSha256 { get; }
    public long SourceFileOffset { get; }
    public Dos16Address InstructionAddress { get; }
    public Dos16ValueOriginKind Kind { get; }

    private Dos16ValueOrigin(string hash, long fileOffset, Dos16Address address,
        Dos16ValueOriginKind kind)
    {
        SourceSha256 = hash;
        SourceFileOffset = fileOffset;
        InstructionAddress = address;
        Kind = kind;
    }

    public static Dos16ValueOrigin Create(string sha256, long fileOffset,
        Dos16Address address, Dos16ValueOriginKind kind)
    {
        if (sha256 is null || sha256.Length != 64 ||
            !sha256.All(char.IsAsciiHexDigit))
            throw new ArgumentException("Full 64-character source SHA-256 required.", nameof(sha256));
        if (fileOffset < 0) throw new ArgumentOutOfRangeException(nameof(fileOffset));
        if (!Enum.IsDefined(kind))
            throw new ArgumentOutOfRangeException(nameof(kind));
        return new Dos16ValueOrigin(sha256.ToLowerInvariant(), fileOffset, address, kind);
    }
}

/// <summary>
/// A value joined with bounded, distinct, source-specific observation labels.
/// Provenance exhaustion is explicit rather than silently treated as complete.
/// </summary>
public sealed class Dos16ValueFact
{
    public const int MaximumOrigins = 8;
    public string SourceSha256 { get; }
    public Dos16AbstractValue Value { get; }
    public IReadOnlyList<Dos16ValueOrigin> Origins { get; }
    public bool OriginsTruncated { get; }

    private Dos16ValueFact(string hash, Dos16AbstractValue value,
        Dos16ValueOrigin[] origins, bool truncated)
    {
        SourceSha256 = hash;
        Value = value;
        Origins = Array.AsReadOnly((Dos16ValueOrigin[])origins.Clone());
        OriginsTruncated = truncated;
    }

    public static Dos16ValueFact Observe(Dos16AbstractValue value,
        Dos16ValueOrigin origin)
    {
        ArgumentNullException.ThrowIfNull(value);
        ArgumentNullException.ThrowIfNull(origin);
        return new Dos16ValueFact(origin.SourceSha256, value, new[] { origin }, false);
    }

    public static Dos16ValueFact Join(Dos16ValueFact left, Dos16ValueFact right)
    {
        ArgumentNullException.ThrowIfNull(left);
        ArgumentNullException.ThrowIfNull(right);
        if (!string.Equals(left.SourceSha256, right.SourceSha256,
                StringComparison.Ordinal))
            throw new InvalidDataException("Abstract-value observations belong to different sources.");
        var origins = left.Origins.Concat(right.Origins)
            .GroupBy(x => (x.SourceFileOffset, x.InstructionAddress, x.Kind))
            .Select(x => x.First())
            .OrderBy(x => x.SourceFileOffset)
            .ThenBy(x => x.InstructionAddress.Segment)
            .ThenBy(x => x.InstructionAddress.Offset)
            .ThenBy(x => x.Kind)
            .Take(MaximumOrigins + 1).ToArray();
        return new Dos16ValueFact(left.SourceSha256,
            Dos16AbstractValue.Join(left.Value, right.Value),
            origins.Take(MaximumOrigins).ToArray(),
            left.OriginsTruncated || right.OriginsTruncated ||
            origins.Length > MaximumOrigins);
    }
}
