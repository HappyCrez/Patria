/*              WebSocket               */
let socket;
let connection_delay = 1000;
function connectToServer() {
    try {
        socket = new WebSocket("ws://127.0.0.1:8080/WebSocket");
        socket.onopen = () => {
            console.log("Connected");
        };

        socket.onmessage = (event) => {
            console.log(event.data);
        }

        socket.onerror = (event) => {
            setTimeout(function () {
                connectToServer();
            }, connection_delay);
        };
    } catch (e) { }
}

function wsSend(data) {

    console.log(data);

    if (!socket.readyState) {
        // If not connected wait
        setTimeout(function () {
            wsSend(data);
        }, 100);
    } else {
        // Send data
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
    setPageState(state.LOGIN_PAGE);
    connectToServer();
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

/*              Login page               */
const username_login = document.getElementById('username_login');
const password_login = document.getElementById('password_login');
const login_submit = document.getElementById('login_submit');

const options = {
    hostname: "SeverPatria",
    method: "POST", // default
};

function loginPageOnKeyDown(event) {
    if (socket) {
        if (!event.shiftKey && event.key == 'Enter') {
            login();
        }
    }
}

login_submit.addEventListener("click", (event) => {
    if (socket) {
        login();
    }
});

function login() {
    wsSend('{"login":"True","username":"' + username_login.value + '","password":"' + password_login.value + '"}');

    // TODO::Listen server to sync dialogs and messages
    // Then parse data and send it to messenger_page
    // data = {
    //     dialogs: ['name2', 'name3'],
    //     name1:
    //     [
    //         'name2:Hello:' + Date.now(),
    //         'name3::Hi!' + Date.now(),
    //         'name2:What are you doing!' + Date.now(),
    //         'name2:I am win a prize today!' + Date.now()
    //     ]
    // };
    messengerPageSetup('stroke');
}

/*              Messenger page               */

const messages = document.getElementById('messages_container');
const bar_scroll = document.getElementById('bar_scroll');
const message_input = document.getElementById('message_input');

let dialogs_list = [];
let reciver = null;

function messengerPageSetup(data) {
    dialogs_list = document.getElementById('dialogs_container').children;
    setPageState(states.MESSENGER_PAGE);
}

// Replace sequences of repeat '\n' or ' ' by one occurence
// After replace all '\n' by tag '<br>'
function formatMessage(message) {
    message = message.replaceAll(message.match(/\n+/), '\n');
    message = message.replaceAll(message.match(/ +/), ' ');
    return message;
}

function appendMessage(message, isSend) {
    message_block = document.createElement('div');
    if (isSend) message_block.classList.add('message_send');
    else message_block.classList.add('message_recv');
    message_block.innerHTML = message;

    time = document.createElement('sub');
    time.classList.add('time');
    date = new Date();
    time_str = date.getHours() + ':' + date.getMinutes();
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

    wsSend('{"send_message":"True","reciver_login":"' + reciver + '","message_content":"' + message + '"}');

    message = message.replace(/\n/g, '<br>');
    appendMessage(message, true)
    message_input.value = '';
    message_input.style.height = 'auto';// Reset the height to auto
}


function recvMessage() {
    if (message_input.value.length > 0)
        appendMessage(formatMessage(message_input.value), false)
    message_input.value = '';
    message_input.style.height = 'auto';// Reset the height to auto
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

function messengerPageOnKeyDown(event) {
    if (search_bar != document.activeElement) {
        message_input.focus();
        if (!event.shiftKey && event.key == 'Enter') {
            sendMessage();
            event.preventDefault();// Disable Enter on input
        }
    }
}

function setActiveDialog(dialog) {
    for (element of dialogs_list) {
        element.classList.remove('active_dialog');
    }
    dialog.classList.add('active_dialog');
}

function messengerPageOnClick(event) {
    if (event.srcElement.classList.contains('dialog') == false)
        return;
    setActiveDialog(event.srcElement);
    reciver = event.srcElement.id;
    console.log(reciver);
}