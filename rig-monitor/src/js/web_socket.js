import { rig_setting } from '../js/rig_setting.js';
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';

let socket = null;
function connect_ws() {
    socket = new WebSocket('http://192.168.68.57/ws');

    socket.onopen = () => {
        console.log('WebSocket connected');
        send_message({ topic: 'control', event: 'refresh' });
    };

    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        // if (data.value.substring(0, 2) !== 'SM') {
        //     console.log('WebSocket message received:', data.value);
        // }
        
        switch (data.value) {
        case '!R;':
            if (!useGlobalStore().isReady) {
                console.log('Rig is ready');
                // for (const key in settings) {
                //     console.log(key, settings[key].value);
                // }
                useGlobalStore().setReady(true);
            }
            break;

        case '!N;':
            if (useGlobalStore().isReady) {
                console.log('Rig is not ready');
                useGlobalStore().setReady(false);
            }
            break;

        default:
            let unavailable = false;
            let command = data.value;
            if (data.value.substring(0,1) === '?') {
                unavailable = true;
                command = data.value.substring(1);
            }
            const rs = rig_setting.fromCommand(command);
            // console.log('rig_setting:', rs);

            if (rs.isMenu) {
                if (unavailable) {
                    console.warn('menu setting unavailable?:', command);
                }
                const rp = rig_property(rs.value.no);
                rp.value = rs.value.value;

                if (rs.value.no === 73) {
                    // console.log('no:', rs.value.no, 'menu:', value);
                    // console.log('menu:', menus[rs.value.no]);
                }
            } else if (rs.name === 'information') {
                // console.log('information:', rs.value);
                rig_property('memory_channel').value = rs.value.memory_channel;
                rig_property('vfo_a').value = rs.value.vfo_a;
                rig_property('mode').value = rs.value.mode;
            } else if (rs.name === 'opposite_band_information') {
                // console.log('opposite_band_information:', rs.value);
            } else {
                const rp = rig_property(rs.name);
                if (rp) {
                    rp.value = rs.value;

                    if (rp.unavailable && !unavailable) {
                        console.log('Has become available:', rs.name);
                    } else if (!rp.unavailable && unavailable) {
                        console.log('Has become unavailable:', rs.name);
                    }
                    rp.unavailable = unavailable;
                } else {
                    console.warn('Unknown setting:', data.value);
                }
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
