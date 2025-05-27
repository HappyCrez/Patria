/*              WebSocket               */
const MIN_DELAY = 1000
let socket;
let connection_delay = MIN_DELAY;
function connectToServer() {
    try {
        socket = new WebSocket("ws://127.0.0.1:8080/WebSocket");
        socket.onopen = () => {
            console.log("Connected");
            connection_delay = MIN_DELAY;
        };

        socket.onmessage = (event) => {

            json = JSON.parse(event.data);
            
            if (json.search_login) {
                console.log("search:" + json.search_login);
                appendDialog(json.search_login);
            } else if (json.send_message) {
                appendMessage(json.send_message, false);

            } else if (json.login) {
                if (json.login == "SUCCESS") {
                    messengerPageSetup(event.data); /* If login success */
                } else {
                    console.log("Access denied");
                }
            }
        }

        socket.onclose = (event) => {
            console.log(event);
        }

        socket.onerror = (event) => {
            console.log("Connection failed, try again in " + connection_delay/1000);
            setTimeout(function () {
                connectToServer();
            }, connection_delay);
            connection_delay *= 2;
        };
    } catch (e) { }
}

function wsSend(data) {
    if (socket.readyState !== WebSocket.OPEN) {
        console.log("Not connected!"); // Show on interface
        return;

        // If not connected wait
        // setTimeout(function () {
        //     wsSend(data);
        // }, 100);
    } else {
        // Send data
        console.log(data);
        socket.send(data);
    }
}

/*              Base logick               */
const login_page = document.getElementById("login_page");
const messenger_page = document.getElementById("messenger_page");

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

    switch(new_state) {
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
    switch(state) {
    case states.LOGIN_PAGE:
        loginPageOnKeyDown(event);
        break;
    case states.MESSENGER_PAGE:
        messengerPageOnKeyDown(event);
        break;
    }
});

document.addEventListener('click', (event) => {
    switch(state) {
    case states.LOGIN_PAGE:

        break;
    case states.MESSENGER_PAGE:
        messengerPageOnClick(event);
        break;
    }
});

window.onload = function() {
    connectToServer();
}

window.onbeforeunload = function() {
    console.log("smthng");
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
        console.log("Login is incorrect");
        return;
    }
    wsSend('{"login":"' + username_login.value + '","password":"' + password_login.value + '"}');
}

/*              Messenger page               */

const messages = document.getElementById('messages_container');
const bar_scroll = document.getElementById('bar_scroll');
const message_input = document.getElementById('message_input');

const dialogs_container = document.getElementById('dialogs_container');
let reciver = null;

function messengerPageSetup(data) {
    setPageState(states.MESSENGER_PAGE);
}

function appendDialog(login) {
    for (element of dialogs_container.children) {

        /* Dialog with gived login exists*/
        if (element.id == login) return;
    }
    let dialog = document.createElement('div');
    dialog.classList.add('dialog');
    dialog.id = login;

    let path_to_image = 'assets/avatar.png"/';
    dialog.innerHTML = '<img src="' + path_to_image + '">' + login;

    dialogs_container.appendChild(dialog);
}

function appendMessage(message, isSend) {
    let message_block = document.createElement('div');
    if (isSend) message_block.classList.add('message_send');
    else message_block.classList.add('message_recv');
    message_block.innerHTML = message;

    time = document.createElement('sub');
    time.classList.add('time');
    date = new Date();
    time_str = date.getHours() + ':' + (date.getMinutes() > 9? date.getMinutes() : "0" + date.getMinutes());
    time.innerHTML = time_str;
    message_block.appendChild(time);

    messages.appendChild(message_block);
}

message_submit.addEventListener('click', (event) => {
    sendMessage();
});

function sendMessage() {
    message = formatMessage(message_input.value);
    if (message_input.value.length <= 0 || reciver == "") return;

    wsSend('{"send_message":"' + reciver + '","message":"' + message + '"}');

    message = message.replace(/\n/g, '<br>');
    appendMessage(message, true)
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

scroll_speed = 30
messages.addEventListener('wheel', (event) => {
    if (messages_top_offset > 0)
        messages_top_offset = 0;
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

search_bar.addEventListener('input', (event)=> {
    if (search_bar.value.length < 1) return;
    wsSend('{"search_login":"' + search_bar.value + '"}');
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

function setActiveDialog(dialog) {
    for (element of dialogs_container.children) {
        element.classList.remove('active_dialog');
    }
    dialog.classList.add('active_dialog');
}

function messengerPageOnClick(event) {
    if (event.srcElement.classList.contains('dialog') == false)
        return;
    setActiveDialog(event.srcElement);
    reciver = event.srcElement.id;
}