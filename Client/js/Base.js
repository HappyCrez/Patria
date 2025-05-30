/*              WebSocket               */
const MIN_DELAY = 1000
let socket;
let connection_delay = MIN_DELAY;

let connection_faild_cnt = 0;
const MAXIMUM_CONNECTION_TRYES = 2;

function connectToServer() {
    try {
        socket = new WebSocket("ws://127.0.0.1:8080/WebSocket");
        socket.onopen = () => {
            //console.log("Connected");

            // Drop connection "onfaild" options
            connection_delay = MIN_DELAY;
            connection_faild_cnt = 0;
        };

        socket.onmessage = (event) => {
            json = JSON.parse(event.data);

            if (json.search_login) {
                //console.log("search:" + json.search_login);
                addDialog(createDialog(json.search_login));
            } else if (json.send_message) {
                json.send_message = json.send_message.replace(/\n/g, '<br>');
                appendMessage(json.sender_login, json.send_message, false);

            } else if (json.login) {
                if (json.login == "SUCCESS") {
                    messengerPageSetup(); // If login success sync data
                } else {
                    //console.log("Access denied");
                }
            }
        }

        socket.onclose = (event) => {
            setPageState(login_page);
        }

        socket.onerror = (event) => {
            location.reload();

            //console.log("Connection failed, try again in " + connection_delay / 1000);
            setTimeout(connectToServer, connection_delay);
            connection_delay *= 2;
            connection_faild_cnt++;
        };
    } catch (e) { }
}

function formatData(data) {
    data = data.replaceAll('\n', '\\n');
    data = data.replaceAll('\r', '\\r');
    data = data.replaceAll('\t', '\\t');
    data = data.replaceAll('\b', '\\b');
    data = data.replaceAll('\"', '\\"');
    return data;
}

function wsSend(data) {
    if (socket.readyState !== WebSocket.OPEN) {
        //console.log("Not connected!"); // Show on interface
        return;
    }
    else {
        // Send data
        //console.log(data);
        socket.send(data);
    }
}

/*              Base logick               */
const login_page = document.getElementById('login_page');
const messenger_page = document.getElementById('messenger_page');

page_list = [login_page, messenger_page];
const states = {
    LOGIN_PAGE: 0,
    MESSENGER_PAGE: 1
}
let state = states.LOGIN_PAGE;

function setVisible(page) {
    page_list.forEach((element) => {
        element.classList.remove('active_page');
    });
    page.classList.add('active_page');
}

function setPageState(new_state) {
    state = new_state;

    switch (new_state) {
        case states.LOGIN_PAGE:
            setVisible(login_page);
            break;
        case states.MESSENGER_PAGE:
            setVisible(messenger_page);
            break;
    }
}

setup();
function setup() {
    setPageState(states.LOGIN_PAGE);
}

document.addEventListener('keydown', (event) => {
    switch (state) {
        case states.LOGIN_PAGE:
            loginPageOnKeyDown(event);
            break;
        case states.MESSENGER_PAGE:
            messengerPageOnKeyDown(event);
            break;
    }
});

document.addEventListener('click', (event) => {
    switch (state) {
        case states.LOGIN_PAGE:

            break;
        case states.MESSENGER_PAGE:
            messengerPageOnClick(event);
            break;
    }
});

window.onload = function () {
    connectToServer();
}

window.onbeforeunload = function () {
    //console.log("smthng");
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.close(1000, "Page unloaded");
    }
}

/*              Login page               */
const username_login = document.getElementById('username_login');
const password_login = document.getElementById('password_login');
const login_submit = document.getElementById('login_submit');

const options = {
    hostname: "SeverPatria",
    method: "POST", // default
};

function loginPageOnKeyDown(event) {
    if (event.key == 'Enter') {
        login();
    }
}

login_submit.addEventListener("click", (event) => {
    login();
});

const regex = /^[a-zA-Z0-9]+$/;

function login() {
    if (regex.test(username_login.value) == false) {
        //console.log("Login is incorrect");
        return;
    }
    client_login = username_login.value;
    wsSend('{"login": "' + username_login.value + '", "password": "' + password_login.value + '"}');
}

/*              Messenger page               */

const messages = document.getElementById('messages_container');
const bar_scroll = document.getElementById('bar_scroll');
const message_input = document.getElementById('message_input');

const dialogs = new Map();
const dialogs_container = document.getElementById('dialogs_container');

const self_chat = "Saved Messages";
let client_login = null;
let reciver = self_chat;
let self_dialog_div = createDialog(self_chat);

function messengerPageSetup(json) {
    setPageState(states.MESSENGER_PAGE);
    dialogs_container.innerHTML = '';
    messages.innerHTML = '';

    addDialog(self_dialog_div);
    setActiveDialog(self_dialog_div);
}

function addDialog(dialog_div) {
    dialogs_container.appendChild(dialog_div);
    dialogs.set(dialog_div.id, []);
}

function createDialog(login, path_to_image = 'assets/avatar.png"/') {
    if (login == null)
        return;

    // Chat with client_login names as "Saved Messages"
    if (login == client_login)
        login = self_chat;

    for (element of dialogs_container.children) {

        // Dialog with gived login exists
        if (element.id == login)
            return;
    }
    let dialog = document.createElement('div');
    dialog.classList.add('dialog');
    dialog.id = login;

    dialog.innerHTML = '<img src="' + path_to_image + '">' + login;
    return dialog;
}

function appendMessage(login, message, isSend) {
    let message_div = document.createElement('div');

    if (isSend)
        message_div.classList.add('message_send');
    else
        message_div.classList.add('message_recv');
    message_div.innerHTML = message;

    time = document.createElement('sub');
    time.classList.add('time');
    date = new Date();
    time_str = date.getHours() + ':' + (date.getMinutes() > 9 ? date.getMinutes() : "0" + date.getMinutes());
    time.innerHTML = time_str;
    message_div.appendChild(time);

    dialogsAddMessage(login, message_div);
}

function dialogsAddMessage(login, message_div) {
    // If dialog not exists just add
    if (dialogs.has(login) == false) {
        addDialog(createDialog(login));
    }
    dialogs.get(login).push(message_div);
    if (reciver == login)
    {
        messages.appendChild(message_div);
    }
}

message_submit.addEventListener('click', (event) => {
    sendMessage();
});

function sendMessage() {
    message = formatMessage(message_input.value);
    if (message_input.value.length <= 0 || reciver == "") {
        return;
    }

    // In this version all self messages don't send on server
    if (reciver != self_chat) {
        wsSend('{"send_message": "' + reciver + '", "message": "' + formatData(message) + '"}');
    }

    message = message.replace(/\n/g, '<br>');
    appendMessage(reciver, message, true)
    message_input.value = '';
    message_input.style.height = 'auto';// Reset the height to auto
}

// Replace sequences of repeat '\n' or ' ' by one occurence
// After replace all '\n' by tag '<br>'
function formatMessage(message) {
    message = message.replaceAll(message.match(/\n+/), '\n');
    message = message.replaceAll(message.match(/ +/), ' ');
    return message;
}

messages_top_offset = 0

scroll_speed = 10
messages.addEventListener('wheel', (event) => {
    if (messages_top_offset > 0)
        messages_top_offset = 0;
    if (messages_top_offset < -messages.clientHeight) {
        messages_top_offset = -messages.clientHeight;
    }
    messages_top_offset += Math.sign(event.deltaY) * scroll_speed;
    messages.style.top = messages_top_offset + 'px';
});

bar_scroll.addEventListener('wheel', (event) => {

});

message_input.addEventListener('input', (event) => {
    //TODO::Expand input footer on write more than 1 row
    // Set the height to the scroll height to make it grow
    //message_input.style.height = message_input.scrollHeight + 'px';
});

search_bar.addEventListener('input', (event) => {
    if (search_bar.value.length < 1) {
        return;
    }
    wsSend('{"search_login": "' + search_bar.value + '"}');
    return;
});

function messengerPageOnKeyDown(event) {
    /* Don't take focus from search bar */
    if (search_bar == document.activeElement) return;

    /* Any key is activate message input */
    message_input.focus();
    if (!event.shiftKey && event.key == 'Enter') {
        sendMessage();
        event.preventDefault(); /* Disable Enter on input */
    }
}

function setDialogHistory(history) {
    messages.innerHTML = ''; // Delete all child elements (messages)
    for (let element of history) {
        messages.appendChild(element);
    }
}

function setActiveDialog(dialog) {
    for (element of dialogs_container.children) {
        element.classList.remove('active_dialog');
    }
    dialog.classList.add('active_dialog');
    setDialogHistory(dialogs.get(dialog.id));
}

function messengerPageOnClick(event) {
    if (event.srcElement.classList.contains('dialog') == false)
        return;
    reciver = event.srcElement.id;
    setActiveDialog(event.srcElement);
}