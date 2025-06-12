'use strict';

import { rig_property } from '@/js/rig_property.js';
import { rig_setting } from '@/js/rig_setting.js';
import { send_message } from '@/js/web_socket.js';
import { conv_freq, is_in_band } from '@/js/freq_utils';

let config = {};
function set_op_mode_config(newConfig) {
    config = { ...newConfig };
}

class freq_range {
    constructor(start, end) {
        this._start = conv_freq(start) || start;
        this._end = conv_freq(end) || end;
    }

    get start() { return this._start; }
    get end() { return this._end; }

    validate() {
        if (this._start === undefined || this._end === undefined) {
            return 'Frequency range is not defined';
        }
        const start_freq = conv_freq(this._start);
        if (!start_freq) {
            return 'Invalid start frequency';
        }

        const end_freq = conv_freq(this._end);
        if (!end_freq) {
            return 'Invalid end frequency';
        }
        if (!is_in_band(start_freq, end_freq)) {
            return 'Frequency range is not in the same band';
        }
        return null;
    }

    asObject() {
        return {
            start: this._start,
            end: this._end,
        };
    }

    static fromJSON(json) {
        if (!json) {
            return null;
        }
        return new freq_range(json.start, json.end);
    }

    static fromObject(obj) {
        if (!obj) {
            return null;
        }
        return new freq_range(obj.start, obj.end);
    }
}

class op_mode {
    constructor(id, name, freq_ranges, order, commands) {
        this._id = id ? parseInt(id, 10) : 0;
        this._name = name ? name.trim() : '';
        this._freq_ranges = freq_ranges || [];
        this._order = order ? parseInt(order, 10) : 0;
        this._commands = commands || [];
    }

    get id() { return this._id; }
    get name() { return this._name; }
    get freq_ranges() { return this._freq_ranges; }
    get order() { return this._order; }
    get commands() { return this._commands; }

    validate(field = null) {
        const errors = {};
        if (!field || field === 'name') {
            if (!this._name) {
                errors.name = 'Name is required';
            } else if (this.name.length > config.MAX_OP_MODE_NAME_LEN) {
                errors.name = 'Name exceeds max length of ' + config.MAX_OP_MODE_NAME_LEN;
            }
        }

        if (!field || field === 'order') {
            const order = parseInt(this._order, 10);
            if (!order && order !== 0) {
                errors.order = 'Order is required';
            } else if (order < 0 || order > config.MAX_ORDER) {
                errors.order = 'Order must be between 0 and ' + config.MAX_ORDER;
            }
        }

        if (!field || field === 'freq_ranges') {
            this._freq_ranges.forEach((range, index) => {
                const error = range.validate();
                if (error) {
                    if (!errors.freq_ranges) {
                        errors.freq_ranges = [];
                    }
                    errors['freq_ranges'][index] = error;
                }
            });
        }

        // commands?
        return errors;
    }

    asUpdate() {
        const freq_ranges = this._freq_ranges
            .map(fr => ({ start: fr.start, end: fr.end }));

        const commands = this._commands
            .map(cmd => rig_property(cmd.name, cmd.value))
            .map(rp => rp.asCommand(rp.value.value))
            .join(';') + ';';

        return {
            id: this._id,
            name: this._name,
            freq_ranges,
            order: this._order,
            commands,
        };
    }

    asObject() {
        const freq_ranges = this._freq_ranges
            .map(fr => ({ start: fr.start, end: fr.end }));

        const commands = this._commands
            .map(cmd => ({ name: cmd.name, value: cmd.value }));

        return {
            id: this._id,
            name: this._name,
            freq_ranges,
            order: this._order,
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

        return new op_mode(
            json.id,
            json.name,
            json.freq_ranges,
            json.order,
            commands
        );
    }

    static fromObject(obj) {
        if (!obj) {
            return null;
        }

        const freq_ranges = obj.freq_ranges ? obj.freq_ranges.map(t => new freq_range(t.start, t.end)) : [];

        const commands = obj.commands
            .map(cmd => ({ name: cmd.name, value: cmd.value }));

        return new op_mode(
            obj.id,
            obj.name,
            freq_ranges,
            obj.order,
            commands
        );
    }

    create() {
        const data = {
            topic: 'op_mode',
            event: 'create',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    update() {
        const data = {
            topic: 'op_mode',
            event: 'update',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    remove() {
        const data = {
            topic: 'op_mode',
            event: 'remove',
            value: { id: this.id },
        };
        send_message(data);
    }
    set_current(name) {
        send_message({ topic: 'controller', event: 'set_op_mode', value: this._name });
    }
}

export {
    config, freq_range, op_mode, set_op_mode_config
};
