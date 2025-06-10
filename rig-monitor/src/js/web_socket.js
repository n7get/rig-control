import { op_mode } from '@/js/op_mode.js';
import { rig_setting } from '../js/rig_setting.js';
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';
import { useGroupsStore } from '@/stores/groups';
import { useOpModeStore } from '@/stores/op_modes';
import { setConfig } from '@/js/op_mode.js'

let socket = null;
function connect_ws() {
    socket = new WebSocket('http://192.168.68.57:8080');

    socket.onopen = () => {
        console.log('WebSocket connected');
        send_message({ topic: 'monitor', event: 'refresh' });
    };

    socket.onmessage = (event) => {
        const message = JSON.parse(event.data);
        // if (message.value.substring(0, 2) !== 'SM') {
            // console.log('WebSocket message received:', message);
        // }
        
        switch(message.topic) {
        case 'monitor':
            handle_monitor(message);
            break;
        case 'op_mode':
            handle_op_mode(message);
            break;
        case 'controller':
            handle_controller(message);
            break;
        default:
            console.log('Unknown message type: ', message);
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

function handle_controller(message) {
    console.log('Controller message received:', message);
    switch (message.event) {
    case 'current_op_mode':
        console.log('Current op mode:', message.value);
        useOpModeStore().set_current_op_mode(message.value);
        break;

    case 'error':
        useGlobalStore().setError(message.value);
        break;

    default:
        console.warn('Unknown controller event:', message);
        break;
    }
}

function handle_op_mode(message) {
    // console.log('Op mode message received:', message);
    switch (message.event) {
    case 'update':
        const om = op_mode.fromJSON(message.value);
        if (om) {
            // console.log('Op mode update:', om.asUpdate());
            useOpModeStore().add_op_mode(om);
        }
        break;

    case 'removed':
        console.log('Op mode removed:', message.value);
        useOpModeStore().remove_op_mode(message.value);
        break;

    case 'refresh':
        if (message.value === 'complete') {
            console.log('Op mode refresh complete');
            send_message({ topic: 'controller', event: 'refresh' });
        }
        break;

    case 'config':
        setConfig(message.value);
        break;

    case 'error':
        useGlobalStore().setError(message.value);
        break;

    default:
        console.warn('Unknown op mode message:', message);
        break;
    }
}

function handle_monitor(message) {
    switch (message.event) {
    case 'update':
        let unavailable = false;
        let command = message.value;
        if (message.value.substring(0,1) === '?') {
            unavailable = true;
            command = message.value.substring(1);
        }

        const rs = rig_setting.fromCommand(command);

        if (rs.isMenu) {
            if (unavailable) {
                console.warn('menu setting unavailable?:', command);
            }
            const rp = rig_property(rs.value.no);
            rp.value = rs.value.value;
        } else if (rs.name === 'information') {
            // console.log('information:', rs.value);
            rig_property('memory_channel').value = rs.value.memory_channel;
            rig_property('vfo_a').value = rs.value.vfo_a;
            rig_property('mode').value = rs.value.mode;
        } else if (rs.name === 'opposite_band_information') {
            // console.log('opposite_band_information:', rs.value);
        } else {
            // if (rs.name === 'radio_status') {
            //     console.log('radio_status:', rs);
            // }
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
                console.warn('Unknown setting:', message.value);
            }
        }
        break;
        
    case 'status':
        switch (message.value) {
        case 'ready':
            if (!useGlobalStore().isReady) {
                useGroupsStore().groups_init();
                console.log('Rig is ready');
                useGlobalStore().setReady(true);

                send_message({ topic: 'op_mode', event: 'refresh' });
            }
            break;

        case 'not_ready':
            if (useGlobalStore().isReady) {
                console.log('Rig is not ready');
                useGlobalStore().setReady(false);
            }
            break;

        default:
            console.warn('Unknown status value:', message.event);
            break;
        }
        break;

    default:
        console.warn('Unknown monitor event:', message);
        break;
    }
}

function send_message(message) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        console.log('send_message', message);
        const json = JSON.stringify(message);
        socket.send(json);
    } else {
        console.warn('WebSocket is not open.');
    }
}

function send_read(read, value) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const rc = rig_setting.of(read, value);
        console.log('send_read', rc.asRead);

        send_message({ topic: 'monitor', event: 'read', value: rc.asRead });
    } else {
        console.warn('WebSocket is not open.');
    }
}

function send_command(command, value) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        const rc = rig_setting.of(command, value);

        send_message({ topic: 'command', event: 'command', value: rc.asSet });
    } else {
        console.warn('WebSocket is not open.');
    }
}

export { connect_ws, send_read, send_command, send_message };
