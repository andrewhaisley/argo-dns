import styles from "./App.module.css";
import { useState, useEffect } from "react";
import HomePage from "./HomePage";
import Login from "./Login";
import { loggedIn, logOut } from "./Util.js";

function logOutRedirect() {
    logOut();
    window.location.href = "/";
}

function App() {

    const [isLoggedIn, setIsLoggedIn] = useState(null);

    useEffect(() => {
        async function check() {
            const result = await loggedIn();
            setIsLoggedIn(result);
        }
        check();
    });

    if (isLoggedIn === null) {
        return <div>Checking login...</div>;
    }

    return (
        <div className={styles.app}>
            {isLoggedIn ? <HomePage onLogout={logOutRedirect} /> : <Login />}
        </div>
    );
}

export default App;
