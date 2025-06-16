'use strict';

import { rig_property } from '@/js/rig_property.js';
import { rig_setting } from '@/js/rig_setting.js';
import { send_message } from '@/js/web_socket.js';

let config = {};
function set_mem_chan_config(newConfig) {
    config = { ...newConfig };
}

class mem_chan {
    constructor(id, name, frequency, op_mode, commands) {
        this._id = id ? parseInt(id, 10) : 0;
        this._name = name ? name.trim() : '';
        this._frequency = frequency ? parseInt(frequency, 10) : 0;
        this._op_mode = op_mode ? op_mode.trim() : '';
        this._commands = commands || [];
    }

    get id() { return this._id; }
    get name() { return this._name; }
    get frequency() { return this._frequency; }
    get op_mode() { return this._op_mode; }
    get commands() { return this._commands; }

    validate(field = null) {
        const errors = {};
        if (!field || field === 'name') {
            if (!this._name) {
                errors.name = 'Name is required';
            } else if (config.MAX_MEM_CHAN_NAME_LEN && this._name.length > config.MAX_MEM_CHAN_NAME_LEN) {
                errors.name = 'Name exceeds max length of ' + config.MAX_MEM_CHAN_NAME_LEN;
            }
        }
        if (!field || field === 'frequency') {
            if (!this._frequency || isNaN(this._frequency)) {
                errors.frequency = 'Frequency is required and must be a number';
            }
        }
        // if (!field || field === 'op_mode') {
        //     if (!this._op_mode) {
        //         errors.op_mode = 'Op mode is required';
        //     }
        // }
        // commands validation can be added as needed
        return errors;
    }

    asUpdate() {
        const commands = this._commands
            .map(cmd => rig_property(cmd.name, cmd.value))
            .map(rp => rp.asCommand(rp.value.value))
            .join(';') + ';';

        return {
            id: this._id,
            name: this._name,
            frequency: this._frequency,
            op_mode: this._op_mode,
            commands,
        };
    }

    asObject() {
        const commands = this._commands
            .map(cmd => ({ name: cmd.name, value: cmd.value }));

        return {
            id: this._id,
            name: this._name,
            frequency: this._frequency,
            op_mode: this._op_mode,
            commands,
        };
    }

    static fromJSON(json) {
        if (!json) {
            return null;
        }

        const commands = json.commands.split(';')
            .filter(cmd => cmd.trim() !== '')
            .map(cmd => rig_setting.fromCommand(cmd + ';'))
            .map(rs => {
                if (rs.isMenu) {
                    return { name: rs.value.no, value: rs.value.value };
                }
                return { name: rs.name, value: rs.value };
            });

        return new mem_chan(
            json.id,
            json.name,
            json.frequency,
            json.op_mode,
            commands,
        );
    }

    static fromObject(obj) {
        if (!obj) {
            return null;
        }

        const commands = obj.commands
            .map(cmd => ({ name: cmd.name, value: cmd.value }));

        return new mem_chan(
            obj.id,
            obj.name,
            obj.frequency,
            obj.op_mode,
            commands
        );
    }

    create() {
        const data = {
            topic: 'mem_chan',
            event: 'create',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    update() {
        const data = {
            topic: 'mem_chan',
            event: 'update',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    remove() {
        const data = {
            topic: 'mem_chan',
            event: 'remove',
            value: { id: this.id },
        };
        send_message(data);
    }
    set_current(name) {
        send_message({ topic: 'controller', event: 'set_mem_chan', value: this._name });
    }
}

export {
    config, mem_chan, set_mem_chan_config
};
