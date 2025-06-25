import { group, set_group_config } from '@/js/group.js';
import { mem_chan, set_mem_chan_config } from '@/js/mem_chan.js';
import { op_mode, set_op_mode_config } from '@/js/op_mode.js';
import { rig_setting } from '../js/rig_setting.js';
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';
import { useGroupsStore } from '@/stores/groups';
import { useMemChanStore } from '@/stores/mem_chans';
import { useOpModeStore } from '@/stores/op_modes';

let socket = null;
const promisees = {};

function connect_ws(ws_url = 'ws://' + window.location.hostname + ':8080', try_dev = true) {
    socket = new WebSocket(ws_url);
    
    socket.onopen = () => {
        console.log('WebSocket connected');
        useGlobalStore().setConnected(true);
        send_message({ topic: 'op_mode', event: 'refresh' });
    };

    socket.onclose = () => {
        console.log('WebSocket disconnected');
        useGlobalStore().setReady(false);
        useGlobalStore().setConnected(false);
    };

    socket.onerror = (error) => {
        const dev_url = 'http://192.168.68.57:8080';

        if (try_dev) {
            console.log('origin URL failed, try development URL: ', dev_url);
            connect_ws(dev_url, false);
            return;
        }

        console.error('WebSocket error:', error);
    };

    socket.onmessage = (event) => {
        const message = JSON.parse(event.data);
        // if (message.value.substring(0, 2) !== 'SM') {
            // console.log('WebSocket message received:', message);
        // }

        if (promisees.hasOwnProperty(message.topic)) {
            switch (message.event) {
            case 'response':
                promisees[message.topic].resolve(message.value);
                delete promisees[message.topic];
                return;

            case 'error':
                promisees[message.topic].reject(message.value);
                delete promisees[message.topic];
                return;
            }
        }
        
        switch(message.topic) {
        case 'controller':
            handle_controller(message);
            break;
        case 'group':
            handle_group(message);
            break;
        case 'mem_chan':
            handle_mem_chan(message);
            break;
        case 'monitor':
            handle_monitor(message);
            break;
        case 'op_mode':
            handle_op_mode(message);
            break;
        default:
            console.log('Unknown message type: ', message);
            break;
        }
    };
};

function handle_controller(message) {
    // console.log('Controller message received:', message);
    switch (message.event) {
    case 'current_op_mode':
        console.log('Current op mode:', message.value);
        useOpModeStore().set_current_op_mode(message.value);
        break;

    case 'current_mem_chan':
        console.log('Current mem chan:', message.value);
        break;

    case 'refresh':
        if (message.value === 'complete') {
            console.log('Controller refresh complete');
            send_message({ topic: 'group', event: 'refresh' });
        }
        break;

    case 'error':
        useGlobalStore().setError(message.value);
        break;

    default:
        console.warn('Unknown controller event:', message);
        break;
    }
}

function handle_group(message) {
    // console.log('Group message received:', message);
    switch (message.event) {
    case 'update':
        const g = group.fromJSON(message.value);
        if (g) {
            // console.log('Group update:', g.asUpdate());
            useGroupsStore().add_group(g);
        }
        break;

    case 'removed':
        // console.log('Group removed:', message.value);
        useGroupsStore().remove_group(message.value);
        break;

    case 'refresh':
        if (message.value === 'complete') {
            console.log('Group refresh complete');
            send_message({ topic: 'monitor', event: 'refresh' });
        }
        break;

    case 'config':
        set_group_config(message.value);
        break;

    default:
        console.warn('Unknown group message:', message);
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
        // console.log('Op mode removed:', message.value);
        useOpModeStore().remove_op_mode(message.value);
        break;

    case 'refresh':
        if (message.value === 'complete') {
            console.log('Op mode refresh complete');
            send_message({ topic: 'mem_chan', event: 'refresh' });
        }
        break;

    case 'config':
        set_op_mode_config(message.value);
        break;

    case 'error':
        useGlobalStore().setError(message.value);
        break;

    default:
        console.warn('Unknown op mode message:', message);
        break;
    }
}

function handle_mem_chan(message) {
    // console.log('Mem chan message received:', message);
    switch (message.event) {
    case 'update':
        const mc = mem_chan.fromJSON(message.value);
        if (mc) {
            // console.log('Mem chan update:', mc.asUpdate());
            useMemChanStore().add_mem_chan(mc);
        }
        break;

    case 'removed':
        // console.log('Mem chan removed:', message.value);
        useMemChanStore().remove_mem_chan(message.value);
        break;

    case 'refresh':
        if (message.value === 'complete') {
            console.log('Mem chan refresh complete');
            send_message({ topic: 'controller', event: 'refresh' });
        }
        break;

    case 'config':
        set_mem_chan_config(message.value);
        break;

    case 'error':
        useGlobalStore().setError(message.value);
        break;

    default:
        console.warn('Unknown mem chan message:', message);
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
                // useGroupsStore().groups_init();
                console.log('Rig is ready');
                useGlobalStore().setReady(true);
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

function send_message_with_response(message) {
    return new Promise((resolve, reject) => {
        if (promisees.hasOwnProperty(message.topic)) {
            console.warn('Promise already exists for topic:', message.topic);
        }
        promisees[message.topic] = { resolve, reject };
        console.log('Promise created for topic:', message.topic);

        send_message(message);
    });
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

export { connect_ws, send_read, send_command, send_message, send_message_with_response };
