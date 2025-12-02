import React, { useState } from "react";
import styles from "./HomePage.module.css";
import { loggedInServer } from "./Util.js";

function HomePage({ onLogout }) {
    const [activeTab, setActiveTab] = useState("Servers");

    const renderContent = () => {
        switch (activeTab) {
            case "Servers":
                return <div className={styles.tab_panel}>Server management tools and list go here.</div>;
            case "Zones":
                return <div className={styles.tab_panel}>Zone management section.</div>;
            case "Server Statistics":
                return <div className={styles.tab_panel}>Performance graphs and metrics appear here.</div>;
            default:
                return null;
        }
    };

    return (
        <div className={styles.homepage_container}>
           <header className={styles.homepage_header}>
                <div className={styles.homepage_left}>
                    <span role="img" className={styles.home_icon}>🏠</span>
                    <a href="/" className={styles.home_link}>Home</a>
                    <span className={styles.homepage_url_label}>Server URL</span>
                    <span className={styles.homepage_server}>{loggedInServer()}</span>
                </div>
                <button className={styles.logout_button} onClick={onLogout}>Logout</button>
            </header>
            <nav className={styles.homepage_tabs}>
                {["Servers", "Zones", "Server Statistics"].map((tab) => (
                    <button
                        key={tab}
                        className={tab === activeTab ? styles.tab_button_active : styles.tab_button}
                        onClick={() => setActiveTab(tab)}>
                        {tab}
                    </button>
                ))}
            </nav>

            <main className={styles.homepage_content}>
                {renderContent()}
            </main>
        </div>
    );
}

export default HomePage;
