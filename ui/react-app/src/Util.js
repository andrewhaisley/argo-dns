export function createBasicAuthHeader(username, password) {
    const credentials = `${username}:${password}`;
    const encoded = btoa(credentials);
    return `Basic ${encoded}`;
}

export async function getDefaultServer() {
    try {
        const response = await fetch("http://localhost:8000/api-port", {
            method: "GET"
        });

        if (response.status === 200) {
            const url = new URL(window.location.href);
            const j = await response.json();
            url.port = j["port"];
            return url.origin;
        } else {
            return "<error>";
        }
    }
    catch (err) {
        return false;
    };
}

export async function loggedIn() {
    var server = localStorage.getItem("server");
    var token = localStorage.getItem("authToken");
    if (!token) {
        return false;
    }
    try {
        const response = await fetch(server + "/1/run", {
            method: "GET",
            headers: { "Authorization": "Bearer " + token }
        });

        if (response.status === 200) {
            return true;
        } else {
            return false;
        }
    }
    catch (err) {
        return false;
    };
}

export function loggedInServer() {
    return localStorage.getItem("server");
}

export function logOut() {
    localStorage.removeItem("server");
    localStorage.removeItem("authToken");
}
