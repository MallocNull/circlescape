class CriticalStop {
    public static redirect(message: string): void {
        window.location.href = "error.html?txt="+ encodeURIComponent(message) +"&rterr";
    }
}