import RigSetting from '../js/rig_settings.js';
import { useSettingsStore } from '@/stores/settings';
import { useStateStore } from '@/stores/state';

let socket = null;
function connectWebSocket() {
    const settings = useSettingsStore().settings;

    socket = new WebSocket('http://192.168.68.57/ws');

    socket.onopen = () => {
        console.log('WebSocket connected');
        sendMessage({ topic: 'control', event: 'refresh' });
    };

    socket.onmessage = (event) => {
        // console.log('WebSocket message received:', event.data);
        const data = JSON.parse(event.data);
        
        switch (data.value) {
        case '!R;':
            if (!useStateStore().isReady) {
                console.log('Rig is ready');
                // for (const key in settings) {
                //     console.log(key, settings[key].value);
                // }
                useStateStore().setReady(true);
            }
            break;

        case '!N;':
            if (useStateStore().isReady) {
                console.log('Rig is not ready');
                useStateStore().setReady(false);
            }
            break;

        default:
            const rs = RigSetting.fromCommand(data.value);
            // console.log('RigSetting:', rs);

            if (rs.isMenu) {
                // Handle menu settings
            } else if (settings.hasOwnProperty(rs.name)) {
                settings[rs.name].value = rs.value;

                if (rs.name === 'vfo_a') {
                    console.log(rs.name, rs.value);
                }
            } else {
                console.warn('Unknown setting:', data.value);
            }
            break;
        }
    };

    socket.onclose = () => {
        console.log('WebSocket disconnected');
    };

    socket.onerror = (error) => {
        console.error('WebSocket error:', error);
    };
};

function sendMessage(message) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const json = JSON.stringify(message);
        socket.send(json);
        console.log('Message sent:', json);
    } else {
        console.warn('WebSocket is not open.');
    }
}

function sendCommand(command, value) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const rc = RigSetting.of(command, value);
        console.log('sendCommand', rc);

        sendMessage({ topic: 'command', command: rc.asSet });
    } else {
        console.warn('WebSocket is not open.');
    }
}

export { connectWebSocket, sendCommand };
