class CriticalStop {
    public static Redirect(message: string): void {
        window.location.href = "error.html?txt="+ encodeURIComponent(message) +"&rterr";
    }
}