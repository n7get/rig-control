import { ref } from 'vue';
import { useMenusStore } from '@/stores/menus.js';
import { useSettingsStore } from '@/stores/settings.js';
import { menu_list, menu_range, menu_suffix } from '@/js/rig_menu.js';
import { rig_setting } from '@/js/rig_setting.js';
import { send_message } from '@/js/web_socket.js';

class rig_property_base {
    constructor(name, value) {
        this._name = name;
        this._value = value;
    }

    get name() { return this._name; }
    get desc() { return this._value.desc; }
    get value() { return this._value.value; }
    get unavailable() { return this._value.unavailable; }
    get suffix() { return ''; }

    set value(value) {
        this._value.value = value;
    }
    set unavailable(value) {
        this._value.unavailable = value;
    }
    setting() { return false; }
    menu() { return false; }
    isBoolean() { return false; }
    update(value) {
        const cmd = this.asCommand(value);
        if (cmd) {
            send_message({ topic: 'monitor', event: 'command', value: cmd });
        }
    }
}

class manu_property extends rig_property_base {
    constructor(name, value) {
        super(name, value);
    }

    get desc() {
        return super.desc + ' (' + this._name + ')';
    }
    get list() { return menu_list(this._name); }
    get range() { return menu_range(this._name); }
    get suffix() { return menu_suffix(this._name) || ''; }
    asCommand(value) {
        return rig_setting.of('menu', { no: this._name, value: value }).asSet;
    }
    get unavailable() { return false; }
    menu() { return true; }
}

class setting_property extends rig_property_base {
    constructor(name, value) {
        super(name, value);
    }

    get list() {
        const rc = rig_setting.of(this._name);
        return rc.list;
    }
    get range() {
        const rc = rig_setting.of(this._name);
        return rc.range;
    }
    asCommand(value) {
        return rig_setting.of(this._name, value).asSet;
    }
    setting() { return true; }
    isBoolean() {
        const rc = rig_setting.of(this._name);
        return rc.isBoolean;
    }
}

    
function rig_property(name, value) {
    if (useSettingsStore().settings.hasOwnProperty(name)) {
        let settings = useSettingsStore().settings[name];
        if (value !== undefined) {
            settings = { ...settings, value: ref(value) };
        }
        return new setting_property(name, settings)
    }

    if (useMenusStore().menus.hasOwnProperty(name)) {
        let menus = useMenusStore().menus[name];
        if (value !== undefined) {
            menus = { ...menus, value: ref(value) };
        }
        return new manu_property(name, menus)
    }

    debugger;
    return undefined
}

function get_property_list(list) {
    const prop_list = list.map((name) => {
        return rig_property(name)
    })
    return prop_list
}
function get_all_settings_list() {
    return Object.keys(useSettingsStore().settings)
}
function get_all_menus_list() {
    return Object.keys(useMenusStore().menus)
}

export {
    rig_property,
    get_property_list,
    get_all_settings_list,
    get_all_menus_list,
};
