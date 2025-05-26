const messages = document.getElementById('messages_container');
const bar_scroll = document.getElementById('bar_scroll');
const message_input = document.getElementById('message_input');

// Replace sequences of repeat '\n' or ' ' by one occurence
// After replace all '\n' by tag '<br>'
function formatMessage(message) {
    message = message.replaceAll(message.match(/\n+/),'\n');
    message = message.replaceAll(message.match(/ +/),' ');
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


document.addEventListener('keydown', (event)=> {
    if (search_bar != document.activeElement)
    {
        message_input.focus();
        if (!event.shiftKey && event.key == 'Enter') {
            sendMessage();
            event.preventDefault();// Disable Enter on input
        }
    }
});

message_submit.addEventListener('click', (event)=> {
    sendMessage();
});

function sendMessage() {
    message = formatMessage(message_input.value);
    if (message_input.value.length <= 0) return;
    
    login = "special_login"
    wsSend('{"send_message":"True","reciver_login":"' + login + '","message_content":"' + message + '"}');
    
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
    messages_top_offset += Math.sign(event.deltaY)*scroll_speed;
    messages.style.top = messages_top_offset + 'px';
});

bar_scroll.addEventListener('wheel', (event) => {
    console.log("321"); 
});


message_input.addEventListener('input', (event) => {
    
    //TODO::Expand input footer on write more than 1 row

    // Set the height to the scroll height to make it grow
    //message_input.style.height = message_input.scrollHeight + 'px';
});

let connection_delay = 2000;
connectToServer();
function connectToServer() {
    try {
        socket = new WebSocket('ws://127.0.0.1:8080/WebSocket');
        socket.onopen = () =>
        {
            console.log('Connected');
            connection_delay = 2000;
        };

        socket.onerror = (err) =>
        {
            console.log('Connection failed. Try again after:' + connection_delay/1000);
            setTimeout(function (){
                connectToServer();
            }, connection_delay);
            connection_delay *= 2;
        };
    } catch (e) {
        console.error('WebSocket error!');
    }
}

function wsSend(data) {
    if(!socket.readyState) {    // If not connected 
        setTimeout(function (){ // Wait
            wsSend(data);
        },100);
    } else {    // Send data
        socket.send(data);
    }
};