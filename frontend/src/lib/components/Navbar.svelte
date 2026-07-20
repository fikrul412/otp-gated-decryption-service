<script>
    import { page } from '$app/state';
</script>

<nav>
    <a href="/" aria-current={page.url.pathname === "/" ? "page" : undefined}>
        Encrypt
    </a>

    <a
        href="/decrypt"
        aria-current={page.url.pathname === "/decrypt" ? "page" : undefined}
    >
        Decrypt
    </a>
</nav>

<style>
    nav {
        display: flex;
        gap: 1rem;
        padding: 1rem;
        border-bottom: 1px solid #ddd;
    }

    a {
        color: black;
        text-decoration: none;
    }

    a[aria-current="page"] {
        font-weight: bold;
        text-decoration: underline;
    }
</style>