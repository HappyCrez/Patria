const messages = document.getElementById('messages_container');
const bar_scroll = document.getElementById('bar_scroll');
const message_input = document.getElementById('message_input');

// Replace sequences of repeat '\n' or ' ' by one occurence
// After replace all '\n' by tag '<br>'
function formatMessage(message) {
    message = message.replaceAll(message.match(/\n+/),'\n');
    message = message.replaceAll(message.match(/ +/),' ');
    message = message.replace(/\n/g, '<br>');
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
    if (message_input.value.length > 0)
        appendMessage(formatMessage(message_input.value), true)
    message_input.value = '';
    message_input.style.height = 'auto';// Reset the height to auto
}

function recvMessage() {
    if (message_input.value.length > 0)
        appendMessage(formatMessage(message_input.value), false)
    message_input.value = '';
    message_input.style.height = 'auto';// Reset the height to auto
}

messages.addEventListener('wheel', (event) => {
    console.log("123"); 
});

bar_scroll.addEventListener('wheel', (event) => {
    console.log("321"); 
});


message_input.addEventListener('input', (event) => {
    
    
    // Set the height to the scroll height to make it grow
    //message_input.style.height = message_input.scrollHeight + 'px';


});