import { rig_setting } from '../js/rig_setting.js';
import { useMenuStore } from '@/stores/menu.js';
import { useSettingsStore } from '@/stores/settings';
import { useStateStore } from '@/stores/state';

let socket = null;
function connect_ws() {
    const menus = useMenuStore().menus;
    const settings = useSettingsStore().settings;

    socket = new WebSocket('http://192.168.68.57/ws');

    socket.onopen = () => {
        console.log('WebSocket connected');
        send_message({ topic: 'control', event: 'refresh' });
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
            const rs = rig_setting.fromCommand(data.value);
            // console.log('rig_setting:', rs);

            if (rs.isMenu) {
                menus[rs.value.no].value = rs.value.value;

                if (rs.value.no === 73) {
                    // console.log('no:', rs.value.no, 'menu:', value);
                    // console.log('menu:', menus[rs.value.no]);
                }
            } else if (settings.hasOwnProperty(rs.name)) {
                settings[rs.name].value = rs.value;

                // if (rs.name === 'monitor') {
                //     console.log(rs.name, rs.value);
                // }
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

function send_message(message) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const json = JSON.stringify(message);
        socket.send(json);
        // console.log('Message sent:', json);
    } else {
        console.warn('WebSocket is not open.');
    }
}

function send_command(command, value) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const rc = rig_setting.of(command, value);
        console.log('send_command', rc.asSet);

        send_message({ topic: 'command', command: rc.asSet });
    } else {
        console.warn('WebSocket is not open.');
    }
}

export { connect_ws, send_command };
