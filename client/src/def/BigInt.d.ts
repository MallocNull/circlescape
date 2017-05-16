declare class bigInt {
    public static readonly one: bigInt;
    public static readonly zero: bigInt;
    public static readonly minusOne: bigInt;

    public static fromArray(digits: Array<number>, base?: any, isNegative?: boolean);
    public static gcd(a: any, b: any): bigInt;
    public static isInstance(a: any): boolean;
    public static lcm(a: any, b: any): bigInt;
    public static max(a: any, b: any): bigInt;
    public static min(a: any, b: any): bigInt;
    public static randBetween(min: any, max: any): bigInt;

    public constructor();
    public constructor(number: any, base?: any);

    public abs(): bigInt;
    public add(x: any): bigInt;
    public and(x: any): bigInt;

    public compare(x: any): bigInt;
    public compareAbs(x: any): bigInt;
    public compareTo(x: any): bigInt;

    public divide(x: any): bigInt;
    public divmod(x: any): bigInt;
    public eq(x: any): bigInt;
    public equals(x: any): bigInt;
    
    public geq(x: any): bigInt;
    public greater(x: any): bigInt;
    public greaterOrEquals(x: any): bigInt;
    public gt(x: any): bigInt;

    public isDivisibleBy(x: any): boolean;
    public isEven(): boolean;
    public isNegative(): boolean;
    public isOdd(): boolean;
    public isPositive(): boolean;
    public isPrime(): boolean;
    public isProbablePrime(iterations?: number): boolean;
    public isUnit(): boolean;
    public isZero(): boolean;

    public leq(x: any): bigInt;
    public lesser(x: any): bigInt;
    public lesserOrEquals(x: any): bigInt;
    public lt(x: any): bigInt;

    public minus(x: any): bigInt;
    public mod(x: any): bigInt;
    public modInv(x: any): bigInt;
    public modPow(exp: any, mod: any): bigInt;
    public multiply(x: any): bigInt;

    public neq(x: any): bigInt;
    public next(): bigInt;
    public not(): bigInt;
    public notEquals(x: any): bigInt;
    
    public or(x: any): bigInt;
    public over(x: any): bigInt;
    
    public plus(x: any): bigInt;
    public pow(x: any): bigInt;
    public prev(): bigInt;

    public remainder(x: any): bigInt;

    public shiftLeft(n: any): bigInt;
    public shiftRight(n: any): bigInt;
    public square(): bigInt;
    public subtract(x: any): bigInt;

    public times(x: any): bigInt;
    public toJSNumber(): number;

    public xor(x: any): bigInt;

    public toString(radix?: any);
    public valueOf();
}