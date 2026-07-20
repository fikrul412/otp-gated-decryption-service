<script>
    import Navbar from '$lib/components/Navbar.svelte';

    let { children } = $props();
</script>

<Navbar />

<main>
    {@render children()}
</main>