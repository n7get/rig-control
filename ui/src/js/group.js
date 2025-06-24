// js/group.js
'use strict';

import { modes_cat } from '@/js/rig_setting.js';
import { send_message } from '@/js/web_socket.js';

let config = {};
function set_group_config(newConfig) {
    config = { ...newConfig };
}

class group {
    constructor(id, name, modes, order, properties) {
        this._id = id ? parseInt(id, 10) : 0;
        this._name = name ? name.trim() : '';
        this._modes = modes ? modes.trim() : '';
        this._order = order ? parseInt(order, 10) : 0;
        this._properties = properties || '';
    }

    get id() { return this._id; }
    get name() { return this._name; }
    get modes() { return this._modes; }
    get order() { return this._order; }
    get properties() { return this._properties; }

    validate(field = null) {
        const errors = {};
        if (!field || field === 'name') {
            if (!this._name) {
                errors.name = 'Name is required';
            } else if (config.MAX_GROUP_NAME_LEN && this._name.length > config.MAX_GROUP_NAME_LEN) {
                errors.name = 'Name exceeds max length of ' + config.MAX_GROUP_NAME_LEN;
            }
        }
        if (!field || field === 'modes') {
            if (!this._modes) {
                errors.modes = 'Modes is required';
            } else {
                const modesArray = this._modes.split(',').map((mode) => mode.trim());
                if (modesArray.length === 0) {
                    errors.modes = 'At least one mode is required';
                } else {
                    for (const mode of modesArray) {
                        if (!modes_cat.hasOwnProperty(mode)) {
                            errors.modes = `Invalid mode: ${mode}`;
                            break;
                        }
                    }
                }
            }
        }
        // properties validation can be added as needed
        return errors;
    }

    asObject() {
        return {
            id: this._id,
            name: this._name,
            modes: this._modes.split(',').map((mode) => mode.trim()),
            order: this._order,
            properties: this._properties.split(',').map((prop) => prop.trim()),
        };
    }

    asUpdate() {
        return {
            id: this._id,
            name: this._name,
            modes: this._modes,
            order: this._order,
            properties: this._properties,
        };
    }

    static fromJSON(json) {
        if (!json) {
            return null;
        }

        return new group(
            json.id,
            json.name,
            json.modes,
            json.order,
            json.properties,
        );
    }

    static fromObject(obj) {
        if (!obj) {
            return null;
        }

        return new group(
            obj.id,
            obj.name,
            obj.modes.join(','),
            obj.order,
            obj.properties.join(','),
        );
    }

    create() {
        const data = {
            topic: 'group',
            event: 'create',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    update() {
        const data = {
            topic: 'group',
            event: 'update',
            value: this.asUpdate(),
        };
        send_message(data);
    }
    remove() {
        const data = {
            topic: 'group',
            event: 'remove',
            value: { id: this.id },
        };
        send_message(data);
    }
}

export {
    config, group, set_group_config
};
