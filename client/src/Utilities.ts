class CriticalStop {
    public static redirect(message: string): void {
        window.location.href = "error.html?txt="+ encodeURIComponent(message) +"&rterr";
    }
}

class Random {
    public static generatePrime(bitCount: number = 512): bigInt {
        var lower = new bigInt(2).pow(bitCount - 1);
        var upper = new bigInt(2).pow(bitCount).prev();
        var prime = new bigInt(4);
        while(!prime.isProbablePrime())
            prime = bigInt.randBetween(lower, upper);

        return prime;
    }
}