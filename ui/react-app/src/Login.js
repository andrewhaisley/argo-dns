import { useState, useEffect } from "react";
import styles from "./Login.module.css";
import { getDefaultServer, createBasicAuthHeader } from "./Util.js";

export default function Login() {
    const [server, setServer] = useState("");
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [error, setError] = useState(null);
    const [loading, setLoading] = useState(false);

    // Load server value asynchronously on mount
    useEffect(() => {
        async function loadServer() {
            try {
                const s = await getDefaultServer();
                setServer(s);
            } catch (e) {
                console.error("Failed to load default server", e);
            }
        }
        loadServer();
    }, []);

    const handleSubmit = async (e) => {
        e.preventDefault();
        setLoading(true);
        setError(null);

        try {
            const response = await fetch(server + "/1/auth_token", {
                method: "GET",
                headers: { Authorization: createBasicAuthHeader(username, password) },
            });

            if (response.status === 200) {
                const data = await response.json();
                localStorage.setItem("server", server);
                localStorage.setItem("authToken", data.token);
                window.location.href = "/";
            } else {
                setError("Invalid username or password");
            }
        } catch (err) {
            setError("Network error");
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className={styles.container}>
        <div className={styles.card}>
        <h2 className={styles.title}>Argo DNS Login</h2>

        <form onSubmit={handleSubmit} className={styles.form}>

        <label className={styles.label}>
            Server URL
            <input
                type="text"
                value={server}
                onChange={(e) => setServer(e.target.value)}
                className={styles.input}
                required
            />
        </label>

        <label className={styles.label}>
            Username
            <input
                type="text"
                value={username}
                onChange={(e) => setUsername(e.target.value)}
                className={styles.input}
                required
            />
        </label>

        <label className={styles.label}>
            Password
            <input
                type="password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                className={styles.input}
                required
            />
        </label>

        {error && <p className={styles.error}>{error}</p>}

        <button type="submit" className={styles.button} disabled={loading}>
            {loading ? "Logging in..." : "Login"}
        </button>

        </form>
        </div>
        </div>
        );
}
