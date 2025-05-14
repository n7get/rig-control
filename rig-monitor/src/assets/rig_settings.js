'use strict;'

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

const cat_to_ctcss = {
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
const ctcss_to_cat = map_reverse(cat_to_ctcss);

const cat_to_CT = {
    '0': 'CTCSS OFF',
    '1': 'CTCSS ENC/DEC',
    '2': 'CTCSS ENC',
    '3': 'DCS ENC/DEC',
    '4': 'DCS ENC'
};
const CT_to_cat = map_reverse(cat_to_CT);

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
    '4': 'AUTO-FAST',
    '5': 'AUTO-MID',
    '6': 'AUTO-SLOW'
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
        mem_chan: intValue(value.substring(0,3)),
        freq: intValue(value.substring(3,12)),
        clarifier: value.substring(12,17),
        rx_clar: value.substring(17, 18) != '0',
        tx_clar: value.substring(18, 19) != '0',
        mode: cat_modes[value.substring(19, 20)] || value.substring(21, 22),
        p7: value.substring(20, 21),
        tone: value.substring(21, 22),
        offset: cat_offset[value.substring(24, 25)],
        tag: value.substring(26),
    };
}

const rig_commands = {
    // 'AB': {                       // TODO: Needs testing
    //     cmd: 'AB',
    //     desc: 'VFO-A_TO_VFO-B',
    //     asSet: sendCmd,
    // },
    'AC': {
        cmd: 'AC',
        desc: 'ANTENNA_TUNER_CONTROL',
        asSetValue() {
            switch(this.value) {
            case 'off': return this._rc.cmd + '000' + ';';
            case 'on':  return this._rc.cmd + '001' + ';';
            case 'tune':return this._rc.cmd + '002' + ';';
            }
        },
        asRead: sendCmd,
        fromAnswer(value) {
            switch(value.substring(2, 3)) {
            case '0':   return 'off';
            case '1':   return 'on';
            case '2':   return 'tune';
            }
        },
    },
    'AG0': {
        cmd: 'AG0',
        desc: 'AF_GAIN',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 4));
        }
    },
    // 'AI': {
    //     cmd: 'AI',
    //     desc: 'AUTO_INFORMATION',
    //     asSetValue: toCatBoolean,
    //     asRead: sendCmd,
    //     fromAnswer: fromBoolean,
    // },
    // 'AM': {                       // TODO: Needs testing
    //     cmd: 'AM',
    //     desc: 'VFO-A_TO_MEMORY_CHANNEL',
    //     asSet: sendCmd,
    // },
    // 'BA': {                       // TODO: Needs testing
    //     cmd: 'BA',
    //     desc: 'VFO-B_TO_VFO-A',
    //     asSet: sendCmd,
    // },
    'BC0': {
        cmd: 'BC0',
        desc: 'AUTO_NOTCH',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1, 2));
        }
    },
    // 'BD0': {                       // TODO: Needs testing
    //     cmd: 'BD0',
    //     desc: 'BAND_DOWN',
    //     asSet: sendCmd,
    // },
    'BI': {
        cmd: 'BI',
        desc: 'BREAK-IN',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'BP00': {                       // TODO: Needs testing
        cmd: 'BP00',
        desc: 'MANUAL_NOTCH_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '00' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(2,3));
        },
    },
    'BP01': {                       // TODO: Needs testing
        cmd: 'BP01',
        desc: 'MANUAL_NOTCH_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value / 10) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            const r = intValue(value);

            if(r === null) {
                return null;
            }

            return r * 10;
        },
    },
    // 'BS': {                       // TODO: Needs testing
    //     cmd: 'BS',
    //     desc: 'BAND_SELECT',
    //     asSetValue() {
    //         if(band_to_cat.hasOwnProperty(this.value)) {
    //             return this._rc.cmd + band_to_cat[this.value] + ';';
    //         }

    //         throw new Error('Unknown band type: ' + this.value);
    //     },
    // },
    // 'BU0': {                       // TODO: Needs testing
    //     cmd: 'BU0',
    //     desc: 'BAND_UP',
    //     asSet: sendCmd,
    // },
    'BY': {                       // TODO: Needs testing
        cmd: 'BY',
        desc: 'BUSY',
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1,2));
        },
    },
    // 'CH0': {                       // TODO: Needs testing
    //     cmd: 'CH0',
    //     desc: 'CHANNEL_UP',
    //     asSet: sendCmd,
    // },
    // 'CH1': {                       // TODO: Needs testing
    //     cmd: 'CH1',
    //     desc: 'CHANNEL_DOWN',
    //     asSet: sendCmd,
    // },
    'CN00': {
        cmd: 'CN00',
        desc: 'CTCSS_TONE_FREQUENCY',
        asSetValue() {
            return this._rc.cmd + ctcss_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_to_ctcss[value];
        },
    },
    'CN01': {
        cmd: 'CN01',
        desc: 'DCS_TONE_FREQUENCY',
        asSetValue() {
            return this._rc.cmd + pad3(cat_to_dcs.indexOf(intValue(this.value))) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            const dcs = cat_to_dcs[intValue(value)];

            return (dcs < 100 ? '0' : '') + dcs.toString();
        },
    },
    'CO00': {                       // TODO: Needs testing
        cmd: 'CO00',
        desc: 'CONTOUR_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '000' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(3, 4))
        },
    },
    'CO01': {                       // TODO: Needs testing
        cmd: 'CO01',
        desc: 'CONTOUR_FREQ',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'CO02': {                       // TODO: Needs testing
        cmd: 'CO02',
        desc: 'APF_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '000' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(3, 4))
        },
    },
    'CO03': {                       // TODO: Needs testing
        cmd: 'CO03',
        desc: 'APF_FREQ',
        asSetValue() {
            return this._rc.cmd + pad4((this.value + 250) / 10) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            const r = intValue(value);

            if(r === null) {
                return null;
            }

            return (r * 10) - 250;
        },
    },
    'CS': {
        cmd: 'CS',
        desc: 'CW_SPOT',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'CT0': {
        cmd: 'CT0',
        desc: 'CTCSS',
        asSetValue() {
            return this._rc.cmd + CT_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_to_CT[value.substring(1, 2)];
        },
    },
    'DA': {
        cmd: 'DA',
        desc: 'DIMMER',
        asSetValue() {
            return this._rc.cmd + '00' + pad2(this.value.led) + pad2(this.value.tft) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return {
                led: intValue(value.substring(2, 4)),
                tft: intValue(value.substring(4))
            };
        },
    },
    // 'DN': {                       // TODO: Needs testing
    //     cmd: 'DN',
    //     desc: 'MIC_DOWN',
    //     asSet: sendCmd,
    // },
    // 'DT0': {
    //     cmd: 'DT0',
    //     desc: 'DATE',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromAnswer(value) {
    //         return value;
    //     },
    // },
    // 'DT1': {
    //     cmd: 'DT1',
    //     desc: 'TIME',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromAnswer(value) {
    //         return value;
    //     },
    // },
    // 'DT2': {
    //     cmd: 'DT2',
    //     desc: 'TIME_DIFFERENTIAL',
    //     asSetValue() {
    //         return this._rc.cmd + this.value + ';';
    //     },
    //     asRead: sendCmd,
    //     fromAnswer(value) {
    //         return value;
    //     },
    // },
    // 'ED': {                       // TODO:
    //     cmd: 'ED',
    //     desc: 'ENCORDER_DOWN',
    // },
    // 'EK': {                       // TODO: Needs testing
    //     cmd: 'EK',
    //     desc: 'ENT_KEY',
    //     asSet: sendCmd,
    // },
    // 'EU': {                       // TODO:
    //     cmd: 'EU',
    //     desc: 'ENCORDER_UP',
    // },
    'EX': {
        cmd: 'EX',
        desc: 'MENU',
        isMenu: true,
        asSetValue() {
            return this._rc.cmd + pad3(this.value.no) + this.value.value + ';';
        },
        asReadValue() {
            return this._rc.cmd + pad3(this.value.no) + ';';
        },
        fromAnswer(value) {
            return {
                no: intValue(value.substring(0, 3)),
                value: value.substring(3)
            };
        },
    },
    'FA': {
        cmd: 'FA',
        desc: 'FREQUENCY_VFO-A',
        asSetValue() {
            return this._rc.cmd + pad9(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value);
        },
    },
    'FB': {
        cmd: 'FB',
        desc: 'FREQUENCY_VFO-B',
        asSetValue() {
            return this._rc.cmd + pad9(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value);
        },
    },
    'FS': {
        cmd: 'FS',
        desc: 'FAST_STEP',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'FT': {                       // TODO:
        cmd: 'FT',
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
        fromAnswer(value) {
            return value === '0' ? 'VFO-A Band Transmitter: TX' : 'VFO-B Band Transmitter: TX';
        }
    },
    'GT': {                       // TODO: Needs testing
        cmd: 'GT',
        desc: 'AGC_FUNCTION',
        asSetValue() {
            node.log('AGC: ' + this.value)
            return this._rc.cmd + agc_to_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_to_agc[value.substring(1, 2)];
        },
    },
    // 'ID': {
    //     cmd: 'ID',
    //     desc: 'IDENTIFICATION',
    //     asRead: sendCmd,
    //     fromAnswer(value) {
    //         return value;
    //     },
    // },
    'IF': {                       // TODO: Needs testing
        cmd: 'IF',
        desc: 'INFORMATION',
        asRead: sendCmd,
        fromAnswer: band_info,
    },
    'IS0': {
        cmd: 'IS0',
        desc: 'IF-SHIFT',
        asSetValue() {
            return this._rc.cmd + this.value + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return value.substring(1);
        },
    },
    // 'KM': {
    //     cmd: 'KM',
    //     desc: 'KEYER_MEMORY',
    //     asSetValue() {
    //         return this._rc.cmd + this.value.no.toString() + this.value.message + ';';
    //     },
    //     asRead() {
    //         return this._rc.cmd + this.value.toString() + ';';
    //     },
    //     fromAnswer(value) {
    //         return {
    //             no: value.substring(0,1),
    //             message: value.substring(1),
    //         };
    //     },
    // },
    'KP': {                       // TODO: Needs testing
        cmd: 'KP',
        desc: 'KEY_PITCH',
        asSetValue() {
            return this._rc.cmd + pad2(this.value / 10 - 30) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return (intValue(value) + 30) * 10;
        },
    },
    'KR': {
        cmd: 'KR',
        desc: 'KEYER',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'KS': {
        cmd: 'KS',
        desc: 'KEY_SPEED',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    // 'KY': {
    //     cmd: 'KY',
    //     desc: 'CW_KEYING',
    //     asSetValue() {
    //         return this._rc.cmd + this.value.toString() + ';';
    //     },
    // },
    'LK': {
        cmd: 'LK',
        desc: 'LOCK',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'LM': {
        cmd: 'LM',
        desc: 'LOAD_MESSEGE',
        asSetValue() {
            throw new Error(this._rc.cmd + ' Not implemented yet');
        },
        asRead(cmd) {
            throw new Error(this._rc.cmd + ' Not implemented yet');
        },
        fromAnswer(value) {
            throw new Error(_rc.cmd + ' Not implemented yet');
        },
    },
    // 'MA': {
    //     cmd: 'MA',
    //     desc: 'MEMORY_CHANNEL_TO_VFO-A',
    // },
    // 'MC': {
    //     cmd: 'MC',
    //     desc: 'MEMORY_CHANNEL',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asRead(cmd) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    'MD0': {
        cmd: 'MD0',
        desc: 'MODE',
        asSetValue() {
            return this._rc.cmd + modes_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_modes[value.substring(1, 2)];
        },
    },
    'MG': {
        cmd: 'MG',
        desc: 'MIC_GAIN',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'ML0': {
        cmd: 'ML0',
        desc: 'MONITOR_ON/OFF',
        asSetValue() {
            return this._rc.cmd + '00' + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(2,3));
        },
    },
    'ML1': {
        cmd: 'ML1',
        desc: 'MONITOR_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value);
        },
    },
    // 'MR': {
    //     cmd: 'MR',
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
        desc: 'METER_SW',
        asSetValue() {
            return this._rc.cmd + meter_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_meter[value.substring(0,1)];
        },
    },
    // 'MT': {
    //     cmd: 'MT',
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
        desc: 'MOX_SET',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'NA0': {
        cmd: 'NA0',
        desc: 'NARROW',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1, 2));
        },
    },
    'NB0': {
        cmd: 'NB0',
        desc: 'NOISE_BLANKER_STATUS',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1, 2));
        },
    },
    'NL0': {
        cmd: 'NL0',
        desc: 'NOISE_BLANKER_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1));
        },
    },
    'NR0': {
        cmd: 'NR0',
        desc: 'NOISE_REDUCTION',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1, 2));
        },
    },
    'OI': {
        cmd: 'OI',
        desc: 'OPPOSITE_BAND_INFORMATION',
        asRead: sendCmd,
        fromAnswer: band_info,
    },
    'OS0': {
        cmd: 'OS0',
        desc: 'OFFSET_(REPEATER_SHIFT)',
        asSetValue() {
            return this._rc.cmd + offset_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_offset[value.substring(1, 2)];
        },
    },
    'PA0': {
        cmd: 'PA0',
        desc: 'PRE-AMP_(IPO)',
        asSetValue() {
            return this._rc.cmd + preamp_cat[this.value] + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return cat_preamp[value.substring(1, 2)];
        },
    },
    // 'PB': {
    //     cmd: 'PB',
    //     desc: 'PLAY_BACK',
    //     asSetValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     asReadValue() {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    //     fromAnswer(value) {
    //         throw new Error(this._rc.cmd + ' Not implemented yet');
    //     },
    // },
    'PC': {
        cmd: 'PC',
        desc: 'POWER_CONTROL',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'PL': {
        cmd: 'PL',
        desc: 'SPEECH_PROCESSOR_LEVEL',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'PR0': {
        cmd: 'PR0',
        desc: 'SPEECH_PROCESSOR_ON/OFF',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'PR1': {
        cmd: 'PR1',
        desc: 'PARAMETRIC_MICROPHONE_EQUALIZER',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    // 'PS': {
    //     cmd: 'PS',
    //     desc: 'POWER_SWITCH',
    //     asSetValue: toCatBoolean,
    //     asRead: sendCmd,
    //     fromAnswer: fromBoolean,
    // },
    // 'QI': {
    //     cmd: 'QI',
    //     desc: 'QMB_STORE',
    //     asSet: sendCmd,
    // },
    // 'QR': {
    //     cmd: 'QR',
    //     desc: 'QMB_RECALL',
    //     asSet: sendCmd,
    // },
    // 'QS': {
    //     cmd: 'QS',
    //     desc: 'QUICK_SPLIT',
    //     asSet: sendCmd,
    // },
    'RA0': {
        cmd: 'RA0',
        desc: 'RF_ATTENUATOR',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return fromBoolean(value.substring(1, 2));
        },
    },
    // 'RC': {
    //     cmd: 'RC',
    //     desc: 'CLAR_CLEAR',
    //     asSet: sendCmd,
    // },
    // 'RD': {
    //     cmd: 'RD',
    //     desc: 'CLAR_DOWN',
    //     asSetValue() {
    //         return this._rc.cmd + pad4(this.value) + ';';
    //     },
    // },
    'RG0': {
        cmd: 'RG0',
        desc: 'RF_GAIN',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 4));
        },
    },
    'RI0': {
        cmd: 'RI0',
        desc: 'RADIO_INFORMATION-HI-SWR',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RI3': {
        cmd: 'RI3',
        desc: 'RADIO_INFORMATION-REC',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RI4': {
        cmd: 'RI4',
        desc: 'RADIO_INFORMATION-PLAY',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RI5': {
        cmd: 'RI5',
        desc: 'RADIO_INFORMATION-VFO-A_TX',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RI6': {
        cmd: 'RI6',
        desc: 'RADIO_INFORMATION-VFO-B_TX',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RI7': {
        cmd: 'RI7',
        desc: 'RADIO_INFORMATION-VFO-A_RX',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RIA': {
        cmd: 'RIA',
        desc: 'RADIO_INFORMATION-TX_LED',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'RL0': {
        cmd: 'RL0',
        desc: 'NOISE_REDUCTION_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad2(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 3));
        },
    },
    'RM0': {
        cmd: 'RM0',
        desc: 'READ_METER-FRONT_PANEL',
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    // 'RM1': {
    //     cmd: 'RM1',
    //     desc: 'READ_METER-S_METER',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    // 'RM2': {
    //     cmd: 'RM2',
    //     desc: 'READ_METER-OTHER_METER',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    // 'RM3': {
    //     cmd: 'RM3',
    //     desc: 'READ_METER-COMP',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    'RM4': {
        cmd: 'RM4',
        desc: 'READ_METER-ALC',
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    // 'RM5': {
    //     cmd: 'RM5',
    //     desc: 'READ_METER-PO',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    'RM6': {
        cmd: 'RM6',
        desc: 'READ_METER-SWR',
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    // 'RM7': {
    //     cmd: 'RM7',
    //     desc: 'READ_METER-ID',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    // 'RM8': {
    //     cmd: 'RM8',
    //     desc: 'READ_METER-VDD',
    //     asRead: sendCmd,
    //     fromAnswer: fromInteger,
    // },
    // 'RS': {
    //     cmd: 'RS',
    //     desc: 'RADIO_STATUS',
    //     asRead: sendCmd,
    //     fromAnswer(value) {
    //         return value === '0' ? 'NORMAL MODE' : 'MENU MODE';
    //     },
    // },
    'RT': {
        cmd: 'RT',
        desc: 'CLAR',
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    // 'RU': {
    //     cmd: 'RU',
    //     desc: 'RX_CLARIFIER_PLUS_OFFSET',
    //     asSetValue() {
    //         return this._rc.cmd + pad4(this.value) + ';';
    //     },
    // },
    // 'SC': {
    //     cmd: 'SC',
    //     desc: 'SCAN',
    //     asSetValue() {
    //         switch(this.value) {
    //         case 'off': return this._rc.cmd + '0' + ';';
    //         case 'up':  return this._rc.cmd + '1' + ';';
    //         case 'down':return this._rc.cmd + '2' + ';';
    //         }
    //     },
    // },
    'SD': {
        cmd: 'SD',
        desc: 'CW_BREAK-IN_DELAY_TIME',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'SH0': {
        cmd: 'SH0',
        desc: 'WIDTH',
        asSetValue() {
            return this._rc.cmd + pad2(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 3));
        },
    },
    'SM0': {
        cmd: 'SM0',
        desc: 'S-METER_READING',
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 4));
        },
    },
    'SQ0': {
        cmd: 'SQ0',
        desc: 'SQUELCH_LEVEL',
        asSetValue() {
            return this._rc.cmd + pad3(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return intValue(value.substring(1, 4));
        },
    },
    // 'SV': {
    //     cmd: 'SV',
    //     desc: 'SWAP_VFO',
    //     asSet: sendCmd,
    // },
    'TS': {
        cmd: 'TS',
        desc: 'TXW',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'TX': {
        cmd: 'TX',
        desc: 'TX_SET',
        asSetValue() {
            return this._rc.cmd + (this.value ? '1' : '0') + ';';
        },
        asRead: sendCmd,
        fromAnswer(value) {
            return !(value === '0');
        },
    },
    'UL': {
        cmd: 'UL',
        desc: 'PLL_UNLOCK_STATUS',
        asRead: sendCmd,
        fromAnswer(value) {
            return value === '0' ? 'Lock' : 'Unlock';
        },
    },
    // 'UP': {
    //     cmd: 'UP',
    //     desc: 'UP',
    //     asSet: sendCmd,
    // },
    'VD': {
        cmd: 'VD',
        desc: 'VOX_DELAY_TIME_/_DATA_VOX_DELAY_TIME',
        asSetValue() {
            return this._rc.cmd + pad4(this.value) + ';';
        },
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    'VG': {
        cmd: 'VG',
        desc: 'VOX_GAIN',
        asSetValue: asInteger,
        asRead: sendCmd,
        fromAnswer: fromInteger,
    },
    // 'VM': {
    //     cmd: 'VM',
    //     desc: 'V/M_KEY_FUNCTION',
    //     asSet: sendCmd,
    // },
    'VX': {
        cmd: 'VX',
        desc: 'VOX_STATUS',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    'XT': {
        cmd: 'XT',
        desc: 'TX_CLAR',
        asSetValue: toCatBoolean,
        asRead: sendCmd,
        fromAnswer: fromBoolean,
    },
    // 'ZI': {
    //     cmd: 'ZI',
    //     desc: 'ZERO_IN',
    //     asSet: sendCmd,
    // }
};
// flow.set('rig_commands', rig_commands);

const rig_desc = {};
for(const [key, value] of Object.entries(rig_commands)) {
    if(!value.hasOwnProperty('desc')) {
        throw new Error(key + ' does not have desc property');
    }
    if(rig_desc.hasOwnProperty(value.desc)) {
        throw new Error(key + ' has duplicate desc property');
    }

    rig_desc[value.desc] = value;
}
// flow.set('rig_desc', rig_desc);

const max_menu_length = Object.keys(rig_commands).map( k => k.length ).reduce( (a, v) => a > v ? a : v );

function logNonPrintable(arg) {
    node.log('arg length: ' + arg.length);
    for (var i = 0; i < arg.length; i++) {
        node.log('arg[' + i + ']: ' + arg.charCodeAt(i));
    }
}

function findRigCommand(arg) {
    if(rig_commands.hasOwnProperty(arg)) {
        return rig_commands[arg];
    }

    if(arg.length <= 2) {
        return null;
    }

    return findRigCommand(arg.substring(0, arg.length - 1));
}

const RigSetting = class {
    constructor(rc, value, raw_answer) {
        this._rc = rc;
        this._value = value;
        this._raw_answer = raw_answer;
    }

    get desc() { return this._rc.desc; }
    get value() { return this._value; }
    get isMenu() { return this._rc.isMenu; }
    get topic() { return this._rc.desc; }

    get asSet() {
        if(this._rc.hasOwnProperty('asSet')) {
			return this._rc.asSet.call(this);
        }

        if(this._rc.hasOwnProperty('asSetValue')) {
            if(isEmptyValue(this._value)) {
                throw new Error(this._rc.desc + ' asSet() requires a value');
            }

			return this._rc.asSetValue.call(this);
        }

        throw new Error(this._rc.desc + ' does not have an asSet() function');
    }

    get asRead() {
        if(this._rc.hasOwnProperty('asRead')) {
			return this._rc.asRead.call(this);
        }

        if(this._rc.hasOwnProperty('asReadValue')) {
            if(isEmptyValue(this._value)) {
                throw new Error(this._rc.desc + ' asRead() requires a value');
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
        if(this._rc.isMenu) {
            return this._rc.cmd + pad3(this._value.no);
        }
        return this._rc.cmd;
    }

    get rawAnswer() {
        if(isEmptyValue(this._raw_answer)) {
            throw new Error(this._rc.cmd + ' does not have original answer');
        }

        return this._raw_answer;
    }

    static of(desc, value) {
        const rc = rig_desc[desc];

        if(!rc) {
            throw new Error('"' + desc + '" not found');
        }

        if(isEmptyValue(value)) {
            new RigSetting(rc, null, null);
        }

        return new RigSetting(rc, value);
    }

    static fromAnswer(arg) {
        let rc = findRigCommand(arg.substring(0, max_menu_length));

        if(rc === null) {

            // It looks like Winlink Express has a bug:
            // At one point it sends a MD0C; with four leadnig
            // ASCII nul (0x0) characters

            node.warn('Failed to find rig command on first try, stripping any non-printable characters and retrying.')
            // logNonPrintable(arg);

            arg = arg.replace(/[^\x20-\x7E]+/g, "");
            rc = findRigCommand(arg.substring(0, max_menu_length));
        }

        if(rc === null) {
            throw new Error('Input not found in rig_commands: "' + arg + '"');
        }

        const value = arg.substring(arg.length - 1) === ';' ?  arg.substring(rc.cmd.length, arg.length - 1) : arg.substring(rc.cmd.length);

        if(!rc.hasOwnProperty('fromAnswer')) {
            throw new Error(rc.cmd + '.fromAnswer() not implemented');
        }

        if(isEmptyValue(value)) {
            return new RigSetting(rc, null, arg);
        }

        return new RigSetting(rc, rc.fromAnswer(value), arg);
    }

    static *getAll() {
        for(const rc of Object.values(rig_commands)) {
            yield new RigSetting(rc, '', '');
        }
    }
};
// global.set('RigSetting', RigSetting);

const rig_state = {};
// flow.set('rig_state', rig_state);

const RigState = {
    findRigSetting(arg) {
        if(rig_state.hasOwnProperty(arg)) {
            return rig_state[arg];
        }

        if(rig_desc.hasOwnProperty(arg)) {
            return rig_state[rig_desc[arg].cmd];
        }

        return null;
    },
    getRigSetting(arg) {
        const rs = RigState.findRigSetting(arg);

        if(rs) {
            return rs;
        }

        throw new Error('Rig state "' + arg + '" not found');
    },

    setRigSetting(rs) {
        rig_state[rs.name] = rs;
    },

    hasChanged(rs) {
        if(!rig_state.hasOwnProperty(rs.name)) {
            return true;
        }

        const crs = rig_state[rs.name];

        return rs.rawAnswer !== crs.rawAnswer;
    },

    getAll() {
        return Object.values(rig_state);
    },
}
// global.set('RigState', RigState);
