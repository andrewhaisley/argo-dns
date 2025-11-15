export var defaultServer = "https://localhost:2000";

export function createBasicAuthHeader(username, password) {
    const credentials = `${username}:${password}`;
    const encoded = btoa(credentials);
    return `Basic ${encoded}`;
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
            console.log("logged in");
            return true;
        } else {
            console.log("not logged in", response.status);
            return false;
        }
    }
    catch (err) {
        return false
    };
}

export function loggedInServer() {
    return localStorage.getItem("server");
}

export function logOut() {
    localStorage.removeItem("server");
    localStorage.removeItem("authToken");
}
