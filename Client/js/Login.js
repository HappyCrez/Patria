const username_login = document.getElementById("username_login");
const password_login = document.getElementById("password_login");

const options = {
    hostname: "SeverPatria",
    method: "POST", // default
};

// Определяем функцию которая принимает в качестве параметров url и данные которые необходимо обработать:
const postData = async (url = "", data = {}) => {
    // Формируем запрос
    const response = await fetch(url, {
        // Метод, если не указывать, будет использоваться GET
        method: "POST",
        // Заголовок запроса
        headers: {
            "Content-Type": "application/json",
        },
        // Данные
        body: JSON.stringify(data),
    });
    return response;
};

function login() {
    postData("/", {
        "login": "True",
        "username": username_login.value,
        "password": password_login.value,
    }).then((data) => {
        console.log(data);
    });
}


login_submit.addEventListener("click", (event) => {
    login();
    document.location.href="/Main.html";
});

document.addEventListener("keydown", (event) => {
    if (event.key == "Enter") {
        login();
    }
});