'use strict';

import { rig_property } from '@/js/rig_property.js';
import { rig_setting } from '@/js/rig_setting.js';
import { send_message } from '@/js/web_socket.js';
import { conv_freq, is_in_band } from '@/js/freq_utils';

let config = {};
function setConfig(newConfig) {
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
        this._commands = commands || {};
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
        return {
            id: this._id,
            name: this._name,
            freq_ranges: this._freq_ranges.map(t => ({ start: t.start, end: t.end })),
            order: this._order,
            commands: Object.entries(this._commands).map(([key, value]) => {
                const rp = rig_property(key);
                return rp.asCommand(value);
            }).reduce((acc, cmd) => {
                return acc + cmd;
            }, ''),
        };
    }

    asObject() {
        const commands = {};
        Object.entries(this._commands).forEach(([key, value]) => {
            commands[key] = value;
        });

        return {
            id: this._id,
            name: this._name,
            freq_ranges: this._freq_ranges.map(t => ({ start: t.start, end: t.end })),
            order: this._order,
            commands: commands,
        };
    }

    static fromJSON(json) {
        if (!json) {
            return null;
        }
        
        const commands = {};
        if (json.commands && json.commands.length > 0) {
            json.commands.split(';').filter(cmd => cmd.trim() !== '').map(cmd => cmd + ';').forEach(cmd => {
                const rs = rig_setting.fromCommand(cmd);
                if (rs.isMenu) {
                    commands[rs.value.no] = rs.value.value;
                } else {
                    commands[rs.name] = rs.value;
                }
            });
        }

        return new op_mode(
            json.id,
            json.name,
            json.freq_ranges,
            json.order,
            commands
        );
    }

    static fromObject(value) {
        if (!value) {
            return null;
        }

        const json = JSON.stringify(value);
        const obj = JSON.parse(json);

        return new op_mode(
            obj.id,
            obj.name,
            obj.freq_ranges ? obj.freq_ranges.map(t => new freq_range(t.start, t.end)) : [],
            obj.order,
            obj.commands
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
        send_message({ topic: 'op_mode', event: 'set_current', value: { id: this._id }});
    }
}

export {
    config, freq_range, op_mode, setConfig
};
