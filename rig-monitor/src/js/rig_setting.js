'use strict;'

import { from_ui, to_ui } from '../js/rig_menu.js';

function isEmptyValue(value) {
    if(typeof(value) === 'string' && value.length === 0) {
        return true;
    }

    if(value === null || value === undefined) {
        return true
    }

    return false;
}

function intValue(value) {
    if(isEmptyValue(value)) {
        return null;
    }
    return parseInt(value, 10);
}

function pad(len) {
    return function(value) {
        let n = typeof(value) === 'string' ? value : value.toString();
value.substring
        while(n.length < len) {
            n = '0' + n;
        }

        return n;
    };
}
const pad2 = pad(2);
const pad3 = pad(3);
const pad4 = pad(4);
const pad9 = pad(9);

function map_reverse(obj) {
    const result = {};

    for(const [key, value] of Object.entries(obj)) {
        result[value] = key;
    }

    return result;
}

const cat_to_ctcss_tone_frequency = {
    '000': '67.0', '001': '69.3', '002': '71.9', '003': '74.4',
    '004': '77.0', '005': '79.7', '006': '82.5', '007': '85.4',
    '008': '88.5', '009': '91.5', '010': '94.8', '011': '97.4',
    '012': '100.0', '013': '103.5', '014': '107.2', '015': '110.9',
    '016': '114.8', '017': '118.8', '018': '123.0', '019': '127.3',
    '020': '131.8', '021': '136.5', '022': '141.3', '023': '146.2',
    '024': '151.4', '025': '156.7', '026': '159.8', '027': '162.2',
    '028': '165.5', '029': '167.9', '030': '171.3', '031': '173.8',
    '032': '177.3', '033': '179.9', '034': '183.5', '035': '186.2',
    '036': '189.9', '037': '192.8', '038': '196.6', '039': '199.5',
    '040': '203.5', '041': '206.5', '042': '210.7', '043': '218.1',
    '044': '225.7', '045': '229.1', '046': '233.6', '047': '241.8',
    '048': '250.3', '049': '254.1'
};
const ctcss_tone_frequency_to_cat = map_reverse(cat_to_ctcss_tone_frequency);

const cat_to_ctcss = {
    '0': 'CTCSS OFF',
    '1': 'CTCSS ENC/DEC',
    '2': 'CTCSS ENC',
    '3': 'DCS ENC/DEC',
    '4': 'DCS ENC'
};
const ctcss_to_cat = map_reverse(cat_to_ctcss);

const cat_to_dcs = [
     23,  25,  26,  31,  32,  36,  43,  47,  51,  53,  54,  65,  71,  72,
     73,  74, 114, 115, 116, 122, 125, 131, 132, 134, 143, 145, 152, 155,
    156, 162, 165, 172, 174, 205, 212, 223, 225, 226, 243, 244, 245, 246,
    251, 252, 255, 261, 263, 265, 266, 271, 274, 306, 311, 315, 325, 331,
    332, 343, 346, 351, 356, 364, 365, 371, 411, 412, 413, 423, 431, 432,
    445, 446, 452, 454, 455, 462, 464, 465, 466, 503, 506, 516, 523, 526,
    532, 546, 565, 606, 612, 624, 627, 631, 632, 654, 662, 664, 703, 712,
    723, 731, 732, 734, 743, 754
];

const band_to_cat = {
    '1.8 MHz': '00',
    '3.5 MHz': '01',
    '5 MHz': '02',
    '7 MHz': '03',
    '10 MHz': '04',
    '14 MHz': '05',
    '18 MHz': '06',
    '21 MHz': '07',
    '24.5 MHz': '08',
    '28 MHz': '09',
    '50 MHz': '10',
    'GEN': '11',
    'MW': '12',
    'AIR': '14',
    '144 MHz': '15',
    '430 MHz': '16'
}

const cat_to_agc = {
    '0': 'OFF',
    '1': 'FAST',
    '2': 'MID',
    '3': 'SLOW',
    '4': 'AUTO',
    '5': 'AUTO',
    '6': 'AUTO',
};
const agc_to_cat = {
    'OFF': '0',
    'FAST': '1',
    'MID': '2',
    'SLOW': '3',
    'AUTO': '4',
};

const cat_meter = {
    '0': 'COMP',
    '1': 'ALC',
    '2': 'PO',
    '3': 'SWR',
    '4': 'ID',
    '5': 'VDD'
};
const meter_cat = map_reverse(cat_meter);

const cat_modes = {
    1: 'LSB',
    2: 'USB',
    3: 'CW',
    4: 'FM',
    5: 'AM',
    6: 'RTTY-LSB',
    7: 'CW-R',
    8: 'DATA-LSB',
    9: 'RTTY-USB',
    A: 'DATA-FM',
    B: 'FM-N',
    C: 'DATA-USB',
    D: 'AM-N',
    E: 'C4FM'
};
const modes_cat = map_reverse(cat_modes);

const cat_offset = {
    0: 'Simplex',
    1: '+',
    2: '-'
};
const offset_cat = map_reverse(cat_offset);

const cat_preamp = {
    '0': 'IPO',
    '1': 'AMP1',
    '2': 'AMP2'
};
const preamp_cat = map_reverse(cat_preamp);

function sendCmd(cmd) {
    return this._rc.cmd + ';';
}

function fromBoolean(value) {
    if(isEmptyValue(value)) {
        return null;
    }
    return value.substring(0, 1) === '1';
}
function toCatBoolean() {
    return this._rc.cmd + (this.value ? '1' : '0') + ';';
}

function fromInteger(value) {
    return intValue(value);
}
function asInteger() {
    return this._rc.cmd + pad3(this.value) + ';';
}

function band_info(value) {
    return {
        raw: value,
        memory_channel: intValue(value.substring(0,3)),
        vfo_a: intValue(value.substring(3,12)),
        clar: value.substring(12,17),
        rx_clar: value.substring(17, 18) != '0',
        tx_clar: value.substring(18, 19) != '0',
        mode: cat_modes[value.substring(19, 20)] || value.substring(21, 22),
        p7: value.substring(20, 21),
        ctcss: cat_to_ctcss[value.substring(21, 22)]   ,
        offset: cat_offset[value.substring(24, 25)],
        tag: value.substring(26),
    };
}

const rig_commands = {
    'AB': {                       // TODO: Needs testing
        cmd: 'AB',
        name: 'vfo_a_to_vfo_b',
        desc: 'VFO-A_TO_VFO-B',
        asSet: sendCmd,
    },
    'AC': {
        cmd: 'AC',
        name: 'antenna_tuner_control',
        desc: 'ANTENNA_TUNER_CONTROL',
        asSetValue() {
            switch(this.value) {
            case 'off': return this._rc.cmd + '000' + ';';
            case 'on':  return this._rc.cmd + '001' + ';';
            case 'tune':return this._rc.cmd + '002' + ';';
            }
        },
        asRead: sendCmd,
        fromCommand(value) {
            switch(value.substring(2, 3)) {
            case '0':   return 'off';
            case '1':   return 'on';
            case '2':   return 'tune';
            }
        },
    },
    'AG0': {
        cmd: 'AG0',
        name: 'af_gain',
	    range: {},
        desc: 'AF_GAIN',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value.substring(1, 4));
        }
    },
    // 'AI': {
    //     cmd: 'AI',
    //     desc: 'AUTO_INFORMATION',
    //     asSetValue: toCatBoolean,
    //     asRead: sendCmd,
    //     fromCommand: fromBoolean,
    // },
    'AM': {                       // TODO: Needs testing
        cmd: 'AM',
        name: 'vfo]a_to_memory_channel',
        desc: 'VFO-A_TO_MEMORY_CHANNEL',
        asSet: sendCmd,
    },
    'BA': {                       // TODO: Needs testing
        cmd: 'BA',
        name: 'vfo_b_to_vfo_a',
        desc: 'VFO-B_TO_VFO-A',
        asSet: sendCmd,
    },
    'BC0': {
        cmd: 'BC0',
        name: 'auto_notch',
        desc: 'AUTO_NOTCH',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        }
    },
    'BD0': {                       // TODO: Needs testing
        cmd: 'BD0',
        name: 'band_down',
        desc: 'BAND_DOWN',
        asSet: sendCmd,
    },
    'BI': {
        cmd: 'BI',
        name: 'break_in',
        desc: 'BREAK-IN',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'BP00': {
        cmd: 'BP00',
        name: 'manual_notch',
        desc: 'MANUAL_NOTCH_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '00' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value.substring(2,3));
        },
    },
    'BP01': {
        cmd: 'BP01',
        name: 'manual_notch_level',
	    range: {min:'10', max:'3200', step:'10'},
        desc: 'MANUAL_NOTCH_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value / 10) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            const r = intValue(value);

            if(r === null) {
                return null;
            }

            return r * 10;
        },
    },
    'BS': {                       // TODO: Needs testing
        cmd: 'BS',
        name: 'band_select',
        desc: 'BAND_SELECT',
        asSetValue() {
            if(band_to_cat.hasOwnProperty(this.value)) {
                return this._rc.cmd + band_to_cat[this.value] + ';';
            }

            throw new Error('Unknown band type: ' + this.value);
        },
    },
    'BU0': {                       // TODO: Needs testing
        cmd: 'BU0',
        name: 'band_up',
        desc: 'BAND_UP',
        asSet: sendCmd,
    },
    'BY': {                       // TODO: Needs testing
        cmd: 'BY',
	    name: 'busy',
        desc: 'BUSY',
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        },
    },
    'CH0': {                       // TODO: Needs testing
        cmd: 'CH0',
        name: 'channel_up',
        desc: 'CHANNEL_UP',
        asSet: sendCmd,
    },
    'CH1': {                       // TODO: Needs testing
        cmd: 'CH1',
        name: 'channel_down',
        desc: 'CHANNEL_DOWN',
        asSet: sendCmd,
    },
    'CN00': {
        cmd: 'CN00',
        name: 'ctcss_tone_frequency',
	    list: Object.keys(ctcss_tone_frequency_to_cat),
        desc: 'CTCSS_TONE_FREQUENCY',
        asSetValue() {
            return this._rc.cmd + ctcss_tone_frequency_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_to_ctcss_tone_frequency[value];
        },
    },
    'CN01': {
        cmd: 'CN01',
        name: 'dcs_tone_frequency',
	    list: cat_to_dcs,
        desc: 'DCS_TONE_FREQUENCY',
        asSetValue() {
            return this._rc.cmd + pad3(cat_to_dcs.indexOf(intValue(this.value))) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            const dcs = cat_to_dcs[intValue(value)];

            return (dcs < 100 ? '0' : '') + dcs.toString();
        },
    },
    'CO00': {                       // TODO: Needs testing
        cmd: 'CO00',
        name: 'contour_on',
        desc: 'CONTOUR_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '000' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value.substring(3, 4))
        },
    },
    'CO01': {                       // TODO: Needs testing
        cmd: 'CO01',
        name: 'contour_freq',
        desc: 'CONTOUR_FREQ',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'CO02': {                       // TODO: Needs testing
        cmd: 'CO02',
        name: 'apf',
        desc: 'APF_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '000' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value.substring(3, 4))
        },
    },
    'CO03': {                       // TODO: Needs testing
        cmd: 'CO03',
        name: 'apf_freq',
        desc: 'APF_FREQ',
        asSetValue() {
            return this._rc.cmd + pad4((this.value + 250) / 10) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            const r = intValue(value);

            if(r === null) {
                return null;
            }

            return (r * 10) - 250;
        },
    },
    'CS': {
        cmd: 'CS',
        name: 'cw_spot',
        desc: 'CW_SPOT',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'CT0': {
        cmd: 'CT0',
        name: 'ctcss',
	    list: Object.keys(ctcss_to_cat),
        desc: 'CTCSS',
        asSetValue() {
            return this._rc.cmd + ctcss_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_to_ctcss[value];
        },
    },
    // 'DA': {
    //     cmd: 'DA',
    //     name: 'dimmer',
    //     name: 'dimmer',
    //     desc: 'DIMMER',
    //     asSetValue() {
    //         return this._rc.cmd + '00' + pad2(this.value.led) + pad2(this.value.tft) + ';';
    //     },
    //     asRead: sendCmd,
    //     fromCommand(value) {
    //         return {
    //             led: intValue(value.substring(2, 4)),
    //             tft: intValue(value.substring(4))
    //         };
    //     },
    // },
    'DN': {                       // TODO: Needs testing
        cmd: 'DN',
        name: 'mic_down',
        desc: 'MIC_DOWN',
        asSet: sendCmd,
    },
    // 'DT0': {
    //     cmd: 'DT0',
    //     name: 'date',
    //     desc: 'DATE',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromCommand(value) {
    //         return value;
    //     },
    // },
    // 'DT1': {
    //     cmd: 'DT1',
    //     name: 'time',
    //     desc: 'TIME',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromCommand(value) {
    //         return value;
    //     },
    // },
    // 'DT2': {
    //     cmd: 'DT2',
    //     name: 'time_differential',
    //     desc: 'TIME_DIFFERENTIAL',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromCommand(value) {
    //         return value;
    //     },
    // },
    // 'ED': {                       // TODO:
    //     cmd: 'ED',
    //     name: 'encorder_down',
    //     desc: 'ENCORDER_DOWN',
    // },
    'EK': {                       // TODO: Needs testing
        cmd: 'EK',
        name: 'ent_key',
        desc: 'ENT_KEY',
        asSet: sendCmd,
    },
    // 'EU': {                       // TODO:
    //     cmd: 'EU',
    //     name: 'encorder_up',
    //     desc: 'ENCORDER_UP',
    // },
    'EX': {
        cmd: 'EX',
	    name: 'menu',
        desc: 'MENU',
        isMenu: true,
        asSetValue() {
            const cmd_value = from_ui(this.value.no, this.value.value);

            return this._rc.cmd + pad3(this.value.no) + cmd_value + ';';
        },
        asReadValue() {
            return this._rc.cmd + pad3(this.value.no) + ';';
        },
        fromCommand(raw_value) {
            const no = intValue(raw_value.substring(0, 3));
            const value = to_ui(no, raw_value.substring(3));

            return { no, value };
        },
    },
    'FA': {
        cmd: 'FA',
        name: 'vfo_a',
        desc: 'FREQUENCY_VFO-A',
        asSetValue() {
            return this._rc.cmd + pad9(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    'FB': {
        cmd: 'FB',
        name: 'vfo_b',
        desc: 'FREQUENCY_VFO-B',
        asSetValue() {
            return this._rc.cmd + pad9(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    'FS': {
        cmd: 'FS',
        name: 'fast_step',
        desc: 'FAST_STEP',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'FT': {                       // TODO:
        cmd: 'FT',
        name: 'function_tx',
        desc: 'FUNCTION_TX',
        asSetValue() {
            if(this.value === 'VFO-A Band Transmitter: TX') {
                return this._rc.cmd + '2' + ';';
            }
            if(this.value === 'VFO-B Band Transmitter: TX') {
                return this._rc.cmd + '3' + ';';
            }
            throw new Error(this._rc.cmd + ' value ' + this.value + ' unknown');
        },
        asRead: sendCmd,
        fromCommand(value) {
            return value === '0' ? 'VFO-A Band Transmitter: TX' : 'VFO-B Band Transmitter: TX';
        }
    },
    'GT0': {                       // TODO: Needs testing
        cmd: 'GT0',
        name: 'agc',
	    list: Object.keys(agc_to_cat),
        desc: 'AGC_FUNCTION',
        asSetValue() {
            return this._rc.cmd + agc_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_to_agc[value];
        },
    },
    // 'ID': {
    //     cmd: 'ID',
    //     name: 'identification',
    //     desc: 'IDENTIFICATION',
    //     asRead: sendCmd,
    //     fromCommand(value) {
    //         return value;
    //     },
    // },
    'IF': {                       // TODO: Needs testing
        cmd: 'IF',
        name: 'information',
        desc: 'INFORMATION',
        asRead: sendCmd,
        fromCommand: band_info,
    },
    'IS0': {
        cmd: 'IS0',
        name: 'if_shift',
	    range: {min:'-1200', max:'1200', step:'20'},
        desc: 'IF-SHIFT',
        asSetValue() {
            const v = parseInt(this.value, 10);
            return this._rc.cmd + (v < 0 ? '-' : '+') + pad4(Math.abs(v)) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return parseInt(value, 10);
        },
    },
    // 'KM': {
    //     cmd: 'KM',
    //     name: 'keyer_memory',
    //     desc: 'KEYER_MEMORY',
    //     asSetValue() {
    //         return this._rc.cmd + this.value.no.toString() + this.value.message + ';';
    //     },
    //     asRead() {
    //         return this._rc.cmd + this.value.toString() + ';';
    //     },
    //     fromCommand(value) {
    //         return {
    //             no: value.substring(0,1),
    //             message: value.substring(1),
    //         };
    //     },
    // },
    'KP': {                       // TODO: Needs testing
        cmd: 'KP',
        name: 'key_pitch',
	    range: {min:'300', max:'1050', step:'10'},
        desc: 'KEY_PITCH',
        asSetValue() {
            return this._rc.cmd + pad2(this.value / 10 - 30) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return (intValue(value) + 30) * 10;
        },
    },
    'KR': {
        cmd: 'KR',
        name: 'keyer',
        desc: 'KEYER',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'KS': {
        cmd: 'KS',
        name: 'key_speed',
	    range: {min:'4', max:'60', step:'1'},
        desc: 'KEY_SPEED',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'KY': {
        cmd: 'KY',
        name: 'cw_keying',
        desc: 'CW_KEYING',
        asSetValue() {
            return this._rc.cmd + this.value.toString() + ';';
        },
    },
    'LK': {
        cmd: 'LK',
        name: 'lock',
        desc: 'LOCK',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'LM': {
        cmd: 'LM',
        name: 'load_messege',
        desc: 'LOAD_MESSEGE',
        asSetValue() {
            throw new Error(this._rc.cmd + ' Not implemented yet');
        },
        asRead(cmd) {
            throw new Error(this._rc.cmd + ' Not implemented yet');
        },
        fromCommand(value) {
            throw new Error(_rc.cmd + ' Not implemented yet');
        },
    },
    'MA': {
        cmd: 'MA',
        name: 'memory_channel_to_vfo_a',
        desc: 'MEMORY_CHANNEL_TO_VFO-A',
        asSet: sendCmd,
    },
    'MC': {
        cmd: 'MC',
        name: 'memory_channel',
        desc: 'MEMORY_CHANNEL',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'MD0': {
        cmd: 'MD0',
        name: 'mode',
	    list: Object.keys(modes_cat),
        desc: 'MODE',
        asSetValue() {
            return this._rc.cmd + modes_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_modes[value];
        },
    },
    'MG': {
        cmd: 'MG',
        name: 'mic_gain',
	    range: {min:'0', max:'100', step:'1'},
        desc: 'MIC_GAIN',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'ML0': {
        cmd: 'ML0',
        name: 'monitor',
        desc: 'MONITOR_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '00' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value.substring(2,3));
        },
    },
    'ML1': {
        cmd: 'ML1',
        name: 'monitor_level',
	    range: {min:'0', max:'100', step:'1'},
        desc: 'MONITOR_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    // 'MR': {
    //     cmd: 'MR',
    //     name: 'memory_channel_read',
    //     desc: 'MEMORY_CHANNEL_READ',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asRead(cmd) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    'MS': {
        cmd: 'MS',
        name: 'meter_sw',
	    list: Object.keys(meter_cat),
        desc: 'METER_SW',
        asSetValue() {
            return this._rc.cmd + meter_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_meter[value];
        },
    },
    // 'MT': {
    //     cmd: 'MT',
    //     name: 'memory_channel_write/tag',
    //     desc: 'MEMORY_CHANNEL_WRITE/TAG',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asRead(cmd) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    // 'MW': {
    //     cmd: 'MW',
    //     name: 'memory_channel_write',
    //     desc: 'MEMORY_CHANNEL_WRITE',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asRead(cmd) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    'MX': {
        cmd: 'MX',
        name: 'mox_set',
        desc: 'MOX_SET',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'NA0': {
        cmd: 'NA0',
        name: 'narrow',
        desc: 'NARROW',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        },
    },
    'NB0': {
        cmd: 'NB0',
        name: 'noise_blanker',
        desc: 'NOISE_BLANKER',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        },
    },
    'NL0': {
        cmd: 'NL0',
        name: 'noise_blanker_level',
	    range: {min:'0', max:'10', step:'1'},
        desc: 'NOISE_BLANKER_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value.substring(1));
        },
    },
    'NR0': {
        cmd: 'NR0',
        name: 'noise_reduction',
        desc: 'NOISE_REDUCTION',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        },
    },
    'OI': {
        cmd: 'OI',
        name: 'opposite_band_information',
        desc: 'OPPOSITE_BAND_INFORMATION',
        asRead: sendCmd,
        fromCommand: band_info,
    },
    'OS0': {
        cmd: 'OS0',
        name: 'offset',
	    list: Object.keys(offset_cat),
        desc: 'OFFSET_(REPEATER_SHIFT)',
        asSetValue() {
            return this._rc.cmd + offset_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_offset[value];
        },
    },
    'PA0': {
        cmd: 'PA0',
        name: 'pre_amp',
	    list: Object.keys(preamp_cat),
        desc: 'PRE-AMP_(IPO)',
        asSetValue() {
            return this._rc.cmd + preamp_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return cat_preamp[value];
        },
    },
    // 'PB': {
    //     cmd: 'PB',
    //     name: 'play_back',
    //     desc: 'PLAY_BACK',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asReadValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     fromCommand(value) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    'PC': {
        cmd: 'PC',
        name: 'power_level',
	    range: {min:'5', max:'100', step:'1'},
        desc: 'POWER_CONTROL',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'PL': {
        cmd: 'PL',
        name: 'speech_processor_level',
	    range: {min:'0', max:'100', step:'1'},
        desc: 'SPEECH_PROCESSOR_LEVEL',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'PR0': {
        cmd: 'PR0',
        name: 'speech_processor',
        desc: 'SPEECH_PROCESSOR_ON/OFF',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'PR1': {
        cmd: 'PR1',
        name: 'parametric_microphone_equalizer',
        desc: 'PARAMETRIC_MICROPHONE_EQUALIZER',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    // 'PS': {
    //     cmd: 'PS',
    //     name: 'power_switch',
    //     desc: 'POWER_SWITCH',
    //     asSetValue: toCatBoolean,
    //     asRead: sendCmd,
    //     fromCommand: fromBoolean,
    // },
    'QI': {
        cmd: 'QI',
        name: 'qmb_store',
        desc: 'QMB_STORE',
        asSet: sendCmd,
    },
    'QR': {
        cmd: 'QR',
        name: 'qmb_recall',
        desc: 'QMB_RECALL',
        asSet: sendCmd,
    },
    'QS': {
        cmd: 'QS',
        name: 'quick_split',
        desc: 'QUICK_SPLIT',
        asSet: sendCmd,
    },
    'RA0': {
        cmd: 'RA0',
        name: 'rf_attenuator',
        desc: 'RF_ATTENUATOR',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return fromBoolean(value);
        },
    },
    'RC': {
        cmd: 'RC',
        name: 'clar_clear',
        desc: 'CLAR_CLEAR',
        asSet: sendCmd,
    },
    'RD': {
        cmd: 'RD',
        name: 'clar_down',
        desc: 'CLAR_DOWN',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
    },
    'RG0': {
        cmd: 'RG0',
        name: 'rf_gain',
	    range: {},
        desc: 'RF_GAIN',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    'RI0': {
        cmd: 'RI0',
        name: 'hi_swr',
        desc: 'RADIO_INFORMATION-HI-SWR',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RI3': {
        cmd: 'RI3',
        name: 'rec',
        desc: 'RADIO_INFORMATION-REC',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RI4': {
        cmd: 'RI4',
        name: 'play',
        desc: 'RADIO_INFORMATION-PLAY',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RI5': {
        cmd: 'RI5',
        name: 'vfo_a_tx',
        desc: 'RADIO_INFORMATION-VFO-A_TX',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RI6': {
        cmd: 'RI6',
        name: 'vfo_b_tx',
        desc: 'RADIO_INFORMATION-VFO-B_TX',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RI7': {
        cmd: 'RI7',
        name: 'vfo_a_rx',
        desc: 'RADIO_INFORMATION-VFO-A_RX',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RIA': {
        cmd: 'RIA',
        name: 'tx_led',
        desc: 'RADIO_INFORMATION-TX_LED',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RL0': {
        cmd: 'RL0',
        name: 'noise_reduction_level',
	    range: {min:'1', max:'15', step:'1'},
        desc: 'NOISE_REDUCTION_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad2(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    'RM0': {
        cmd: 'RM0',
        name: 'read_meter_front_panel',
        desc: 'READ_METER-FRONT_PANEL',
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    // 'RM1': {
    //     cmd: 'RM1',
    //     name: 'read_meter_s_meter',
    //     desc: 'READ_METER-S_METER',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    // 'RM2': {
    //     cmd: 'RM2',
    //     name: 'read_meter_other_meter',
    //     desc: 'READ_METER-OTHER_METER',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    // 'RM3': {
    //     cmd: 'RM3',
    //     name: 'read_meter_comp',
    //     desc: 'READ_METER-COMP',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    'RM4': {
        cmd: 'RM4',
        name: 'read_meter_alc',
        desc: 'READ_METER-ALC',
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    // 'RM5': {
    //     cmd: 'RM5',
    //     name: 'read_meter_po',
    //     desc: 'READ_METER-PO',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    'RM6': {
        cmd: 'RM6',
        name: 'read_meter_swr',
        desc: 'READ_METER-SWR',
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    // 'RM7': {
    //     cmd: 'RM7',
    //     name: 'read_meter_id',
    //     desc: 'READ_METER-ID',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    // 'RM8': {
    //     cmd: 'RM8',
    //     name: 'read_meter_vdd',
    //     desc: 'READ_METER-VDD',
    //     asRead: sendCmd,
    //     fromCommand: fromInteger,
    // },
    'RS': {
        cmd: 'RS',
        name: 'radio_status',
        desc: 'RADIO_STATUS',
        asRead: sendCmd,
        fromCommand(value) {
            return value === '0' ? 'NORMAL MODE' : 'MENU MODE';
        },
    },
    'RT': {
        cmd: 'RT',
        name: 'clar',
        desc: 'CLAR',
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'RU': {
        cmd: 'RU',
        name: 'rx_clarifier_plus_offset',
        desc: 'RX_CLARIFIER_PLUS_OFFSET',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
    },
    'SC': {
        cmd: 'SC',
        name: 'scan',
        desc: 'SCAN',
        asSetValue() {
            switch(this.value) {
            case 'off': return this._rc.cmd + '0' + ';';
            case 'up':  return this._rc.cmd + '1' + ';';
            case 'down':return this._rc.cmd + '2' + ';';
            }
        },
    },
    'SD': {
        cmd: 'SD',
        name: 'cw_break_in_delay_time',
        desc: 'CW_BREAK-IN_DELAY_TIME',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'SH0': {
        cmd: 'SH0',
        name: 'width',
        desc: 'WIDTH',
        asSetValue() {
            return this._rc.cmd + pad2(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value.substring(1, 3));
        },
    },
    'SM0': {
        cmd: 'SM0',
        name: 's_meter_reading',
        desc: 'S-METER_READING',
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value.substring(1, 4));
        },
    },
    'SQ0': {
        cmd: 'SQ0',
        name: 'squelch_level',
	    range: {min:'0', max:'100', step:'1'},
        desc: 'SQUELCH_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return intValue(value);
        },
    },
    'SV': {
        cmd: 'SV',
        name: 'swap_vfo',
        desc: 'SWAP_VFO',
        asSet: sendCmd,
    },
    'TS': {
        cmd: 'TS',
        name: 'txw',
        desc: 'TXW',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'TX': {
        cmd: 'TX',
        name: 'transmit',
        desc: 'TX_SET',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromCommand(value) {
            return !(value === '0');
        },
    },
    'UL': {
        cmd: 'UL',
        name: 'pll_unlock_status',
        desc: 'PLL_UNLOCK_STATUS',
        asRead: sendCmd,
        fromCommand(value) {
            return value === '0' ? 'Lock' : 'Unlock';
        },
    },
    'UP': {
        cmd: 'UP',
        name: 'up',
        desc: 'UP',
        asSet: sendCmd,
    },
    'VD': {
        cmd: 'VD',
        name: 'vox_delay_time',
        desc: 'VOX_DELAY_TIME_/_DATA_VOX_DELAY_TIME',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'VG': {
        cmd: 'VG',
        name: 'vox_gain',
	    range: {min:'0', max:'100', step:'1'},
        desc: 'VOX_GAIN',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromCommand: fromInteger,
    },
    'VM': {
        cmd: 'VM',
        name: 'v_m_key_function',
        desc: 'V/M_KEY_FUNCTION',
        asSet: sendCmd,
    },
    'VX': {
        cmd: 'VX',
        name: 'vox',
        desc: 'VOX',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'XT': {
        cmd: 'XT',
        name: 'tx_clar',
        desc: 'TX_CLAR',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromCommand: fromBoolean,
    },
    'ZI': {
        cmd: 'ZI',
        name: 'zero_in',
        desc: 'ZERO_IN',
        asSet: sendCmd,
    }
};

const commands = Object.values(rig_commands);
const rig_names = {};
for(const [key, value] of Object.entries(rig_commands)) {
    if(!value.hasOwnProperty('name')) {
        throw new Error(key + ' does not have name property');
    }
    if(rig_names.hasOwnProperty(value.name)) {
        throw new Error(key + ' has duplicate name property');
    }

    rig_names[value.name] = value;
}

const max_menu_length = Object.keys(rig_commands).map( k => k.length ).reduce( (a, v) => a > v ? a : v );

function findRigCommand(arg) {
    for (const rc of commands) {
        if(rc.cmd === arg.substring(0, rc.cmd.length)) {
            return rc;
        }
    }
    return null;
    // if(rig_commands.hasOwnProperty(arg)) {
    //     return rig_commands[arg];
    // }

    // if(arg.length <= 2) {
    //     return null;
    // }

    // return findRigCommand(arg.substring(0, arg.length - 1));
}

const rig_setting = class {
    constructor(rc, value, command) {
        this._rc = rc;
        this._value = value;
        this._command = command;
    }

    get name() { return this._rc.name; }
    get value() { return this._value; }
    get list() { return this._rc.hasOwnProperty('list') ? this._rc.list : null; }
    get range() { return this._rc.hasOwnProperty('range') ? this._rc.range : null; }
    get isMenu() { return this._rc.isMenu; }

    get asSet() {
        if(this._rc.hasOwnProperty('asSet')) {
			return this._rc.asSet.call(this);
        }

        if(this._rc.hasOwnProperty('asSetValue')) {
            if(isEmptyValue(this._value)) {
                throw new Error(this._rc.name + ' asSet() requires a value');
            }

			return this._rc.asSetValue.call(this);
        }

        throw new Error(this._rc.name + ' does not have an asSet() function');
    }

    get asRead() {
        if(this._rc.hasOwnProperty('asRead')) {
			return this._rc.asRead.call(this);
        }

        if(this._rc.hasOwnProperty('asReadValue')) {
            if(isEmptyValue(this._value)) {
                throw new Error(this._rc.name + ' asRead() requires a value');
            }

			return this._rc.asReadValue.call(this);
        }

        throw new Error(this._rc.cmd + ' does not have an asRead() function');
    }

    get autoInit() { 
        return this._rc.autoInit || (this._rc.cmd === 'MT' && !isEmptyValue(this._value));
    }
    
    get sendToRig() {
        const hasValue = !isEmptyValue(this._value),
              hasSet = this._rc.hasOwnProperty('asSet'),
              hasSetValue = this._rc.hasOwnProperty('asSetValue'),
              hasRead = this._rc.hasOwnProperty('asRead'),
              hasReadValue = this._rc.hasOwnProperty('asReadValue');

        if(this.isMenu) {
            return !isEmptyValue(this.value.value);
        }

        if(hasValue && hasSetValue && hasRead) {
            return hasValue;
        }

        if((hasSet || hasSetValue) && !(hasRead || hasReadValue)) {
            return true;
        }

        return false;
    }

    get name() {
        // if(this._rc.isMenu) {
        //     return this._rc.cmd + pad3(this._value.no);
        // }
        return this._rc.name;
    }

    get rawCommand() {
        if(isEmptyValue(this._command)) {
            throw new Error(this._rc.cmd + ' does not have original answer');
        }

        return this._command;
    }

    static of(name, value) {
        const rc = rig_names[name];

        if(!rc) {
            throw new Error('"' + name + '" not found');
        }

        if(isEmptyValue(value)) {
            new rig_setting(rc, null, null);
        }

        return new rig_setting(rc, value);
    }

    // static toMenuCommand(no, value) {
    //     if(!Number.isInteger(no) || no < 1 || no > 153) {
    //         throw new Error('Menu number must be an integer between 1 and 153');
    //     }

    //     if(value === null || value === undefined) {
    //         throw new Error('Menu value must not be null or undefined');
    //     }

    //     return new rig_setting(rig_commands['EX'], { no: no, value: value });
    // }

    static fromCommand(arg) {
        let rc = null;
        if (arg.startsWith('EX')) {
            rc = rig_commands['EX'];
        } else {
            rc = findRigCommand(arg);
        }

        if(rc === null) {
            throw new Error('Command not found in rig_commands: "' + arg + '"');
        }

        const value = arg.substring(rc.cmd.length, arg.length - 1);

        if(!rc.hasOwnProperty('fromCommand')) {
            throw new Error(rc.cmd + '.fromCommand() not implemented');
        }

        if(isEmptyValue(value)) {
            return new rig_setting(rc, null, arg);
        }

        return new rig_setting(rc, rc.fromCommand(value), arg);
    }

    static *getAll() {
        for(const rc of Object.values(rig_commands)) {
            yield new rig_setting(rc, '', '');
        }
    }
};

export {
    agc_to_cat,
    cat_preamp, preamp_cat,
    rig_setting,
};
