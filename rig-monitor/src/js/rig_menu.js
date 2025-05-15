'use strict';

function isEmptyValue(value) {
    if (typeof (value) === 'string' && value.length === 0) {
        return true;
    }

    if (value === null || value === undefined) {
        return true
    }

    return false;
}

function pad(len) {
    return function (value) {
        if (isEmptyValue(value)) {
            return null;
        }

        let n = typeof (value) === 'string' ? value : value.toString();

        while (n.length < len) {
            n = '0' + n;
        }

        return n;
    };
}
const pad1 = pad(1);
const pad2 = pad(2);
const pad3 = pad(3);
const pad4 = pad(4);
const pad9 = pad(9);

// UI Iterface functions

function defaultToUi(value) {
    return value;
}
function defaultFromUi(value) {
    return value;
}

function fromUiMaxPower(max) {
    return function (value) {
        if (value < 5) {
            return '005';
        }
        if (value >= max) {
            return pad3(max);
        }
        return pad3(value);
    }
}

function toUiInt(value) {
    return parseInt(value, 10).toString();
}

function fromUiIntBy20(value) {
    if (isEmptyValue(value)) {
        return null;
    }

    return pad4(value - (v % 20));
}

function toCutFreq(value, offset) {
    if (value === '00') {
        return 'OFF';
    }

    return ((parseInt(value, 10) + offset) * 50).toString();
}
function fromCutFreq(value, offset) {
    if (value === 'OFF') {
        return '00';
    }

    return pad2(Math.floor((value / 50)) - offset);
}
function toUiLCutFreq(value) {
    return toCutFreq(value, 1);
}
function fromUiLCutFreq(value) {
    return fromCutFreq(value, 1);
}
function toUiHCutFreq(value) {
    return toCutFreq(value, 13);
}
function fromUiHCutFreq(value) {
    return fromCutFreq(value, 13);
}

// Start of maps

const mic_select_map = {
    '0': 'MIC',
    '1': 'REAR',
};

const ptt_select_map = {
    '0': 'DAKY',
    '1': 'RTS',
    '2': 'DTR',
};

const port_select_map = {
    '0': 'DATA',
    '1': 'USB',
};

const slope_map = {
    '0': '6 dB',
    '1': '18 dB',
};

const cw_memory_map = {
    '0': 'TEXT',
    '1': 'MESSAGE',
};

const baud_rate_map = {
    '0': '4800',
    '1': '9600',
    '2': '19200',
    '3': '38400',
};

const serial_timeout_map = {
    '0': '10 ms',
    '1': '100 ms',
    '2': '1000 ms',
    '3': '3000 ms',
};

const enable_disable_map = {
    '0': 'DISABLE',
    '1': 'ENABLE',
};

const on_off_map = {
    '0': 'OFF',
    '1': 'ON',
};

const normal_reverse_map = {
    '0': 'NORMAL',
    '1': 'REVERSE',
};

const standard_colors_map = {
    '0': 'BLUE',
    '1': 'GRAY',
    '2': 'GREEN',
    '3': 'ORANGE',
    '4': 'PURPLE',
    '5': 'RED',
    '6': 'SKY BLUE',
};


const prmtrc_eq1_freq_map = {
    '00': 'OFF',
    '01': '100 Hz',
    '02': '200 Hz',
    '03': '300 Hz',
    '04': '400 Hz',
    '05': '500 Hz',
    '06': '600 Hz',
    '07': '700 Hz',
};

const prmtrc_eq2_freq_map = {
    '00': 'OFF',
    '01': '700 Hz',
    '02': '800 Hz',
    '03': '900 Hz',
    '04': '1000 Hz',
    '05': '1100 Hz',
    '06': '1200 Hz',
    '07': '1300 Hz',
    '08': '1400 Hz',
    '09': '1500 Hz',
};

const prmtrc_eq3_freq_map = {
    '00': 'OFF',
    '01': '1500 Hz',
    '02': '1600 Hz',
    '03': '1700 Hz',
    '04': '1800 Hz',
    '05': '1900 Hz',
    '06': '2000 Hz',
    '07': '2100 Hz',
    '08': '2200 Hz',
    '09': '2300 Hz',
    '10': '2400 Hz',
    '11': '2500 Hz',
    '12': '2600 Hz',
    '13': '2700 Hz',
    '14': '2800 Hz',
    '15': '2900 Hz',
    '16': '3000 Hz',
    '17': '3100 Hz',
    '18': '3800 Hz',
};

// End of maps

const mi_table = {
    1: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
    },
    2: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
    },
    3: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
    },
    4: {
        rig_ui_map: {
            '0': 'SCOPE',
            '1': 'FUNCTION',
        },
    },
    5: {
        toUi: toUiInt,
        fromUi: pad1,
    },
    6: {
        rig_ui_map: standard_colors_map,
    },
    7: {
        rig_ui_map: {
            '0': '1',
            '1': '2',
        },
    },
    8: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    9: {
        rig_ui_map: {
            '0': 'OFF',
            '1': '0.5sec',
            '2': '1.0sec',
            '3': '2.0sec',
        },
    },
    10: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    11: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    12: {
        rig_ui_map: {
            '0': 'OFF',
            '1': 'BUG',
            '2': 'ELEKEY-A',
            '3': 'ELEKEY-B',
            '4': 'ELEKEY-Y',
            '5': 'ACS',
        },
    },
    13: {
        rig_ui_map: normal_reverse_map,
    },
    14: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    15: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    16: {
        rig_ui_map: {
            '0': '1290',
            '1': 'AUNO',
            '2': 'AUNT',
            '3': 'A2NO',
            '4': 'A2NT',
            '5': '12NO',
            '6': '12NT',
        },
    },
    17: {
        toUi: toUiInt,
        fromUi: pad4,
    },
    18: {
        rig_ui_map: cw_memory_map,
    },
    19: {
        rig_ui_map: cw_memory_map,
    },
    20: {
        rig_ui_map: cw_memory_map,
    },
    21: {
        rig_ui_map: cw_memory_map,
    },
    22: {
        rig_ui_map: cw_memory_map,
    },
    23: {
        rig_ui_map: {
            '0': '1 ms',
            '1': '3 ms',
            '2': '10 ms',
        },
    },
    24: {
        rig_ui_map: {
            '0': '10 dB',
            '1': '30 dB',
            '2': '50 dB',
        },
    },
    25: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    26: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    27: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    28: {
        rig_ui_map: {
            '0': 'GPS1',
            '1': 'GPS2',
            '2': 'RS232C',
        },
    },
    29: {
        rig_ui_map: baud_rate_map,
    },
    30: {
        rig_ui_map: serial_timeout_map,
    },
    31: {
        rig_ui_map: baud_rate_map,
    },
    32: {
        rig_ui_map: serial_timeout_map,
    },
    33: {
        rig_ui_map: enable_disable_map,
    },
    34: {
        rig_ui_map: enable_disable_map,
    },
    35: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    36: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    37: {
        rig_ui_map: enable_disable_map,
    },
    38: {
        rig_ui_map: {
            '0': 'PAUSE',
            '1': 'TIME',
        },
    },
    39: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    40: {
        rig_ui_map: {
            '0': 'RX',
            '1': 'TX',
            '2': 'TRX',
        },
    },
    41: {
        toUi: toUiLCutFreq,
        fromUi: fromUiLCutFreq,
    },
    42: {
        rig_ui_map: slope_map,
    },
    43: {
        toUi: toUiHCutFreq,
        fromUi: fromUiHCutFreq,
    },
    44: {
        rig_ui_map: slope_map,
    },
    45: {
        rig_ui_map: mic_select_map,
    },
    46: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    47: {
        rig_ui_map: ptt_select_map,
    },
    48: {
        rig_ui_map: port_select_map,
    },
    49: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    50: {
        toUi: toUiLCutFreq,
        fromUi: fromUiLCutFreq,
    },
    51: {
        rig_ui_map: slope_map,
    },
    52: {
        toUi: toUiHCutFreq,
        fromUi: fromUiHCutFreq,
    },
    53: {
        rig_ui_map: slope_map,
    },
    54: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    55: {
        rig_ui_map: {
            '0': 'OFF',
            '1': '50 MHz',
            '2': 'ON',
        },
    },
    56: {
        rig_ui_map: {
            '0': 'SEMI BREAK-IN',
            '1': 'FULL BREAK-IN',
        },
    },
    57: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    58: {
        rig_ui_map: {
            '0': '1 ms',
            '1': '2 ms',
            '2': '4 ms',
            '3': '6 ms',
        },
    },
    59: {
        rig_ui_map: {
            '0': 'DIRECT FREQ',
            '1': 'PITCH OFFSET',
        },
    },
    60: {
        rig_ui_map: {
            '0': 'OFF',
            '1': 'DAKY',
            '2': 'RTS',
            '3': 'DTR',
        },
    },
    61: {
        rig_ui_map: {
            '0': '15 ms',
            '1': '20 ms',
            '2': '25 ms',
            '3': '30 ms',
        },
    },
    62: {
        rig_ui_map: {
            '0': 'PSK',
            '1': 'OTHER',
        },
    },
    63: {
        rig_ui_map: {
            '0': '1000 Hz',
            '1': '1500 hz',
            '2': '2000 Hz',
        },
    },
    64: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    65: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    66: {
        toUi: toUiLCutFreq,
        fromUi: fromUiLCutFreq,
    },
    67: {
        rig_ui_map: slope_map,
    },
    68: {
        toUi: toUiHCutFreq,
        fromUi: fromUiHCutFreq,
    },
    69: {
        rig_ui_map: slope_map,
    },
    70: {
        rig_ui_map: mic_select_map,
    },
    71: {
        rig_ui_map: ptt_select_map,
    },
    72: {
        rig_ui_map: port_select_map,
    },
    73: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    74: {
        rig_ui_map: mic_select_map,
    },
    75: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    76: {
        rig_ui_map: ptt_select_map,
    },
    77: {
        rig_ui_map: port_select_map,
    },
    78: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    79: {
        rig_ui_map: {
            '0': '1200',
            '1': '9600',
        },
    },
    80: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    81: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    82: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    83: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    84: {
        rig_ui_map: on_off_map,
    },
    85: {
        rig_ui_map: on_off_map,
    },
    86: {
        rig_ui_map: {
            '0': 'Tn-Rn',
            '1': 'Tn-Riv',
            '2': 'Tiv-Rn',
            '3': 'Tiv-Riv',
        },
    },
    87: {
        toUi: defaultToUi,
    },
    88: {
        rig_ui_map: {
            '0': 'DISTANCE',
            '1': 'STRENGTH',
        },
    },
    89: {
        rig_ui_map: {
            '0': 'km',
            '1': 'mile',
        },
    },
    90: {
        rig_ui_map: {
            '0': 'AUTO',
            '1': 'MANUAL',
            '2': 'DN',
            '3': 'VW',
            '4': 'ANALOG',
        },
    },
    91: {
        rig_ui_map: on_off_map,
    },
    92: {
        toUi: toUiLCutFreq,
        fromUi: fromUiLCutFreq,
    },
    93: {
        rig_ui_map: slope_map,
    },
    94: {
        toUi: toUiHCutFreq,
        fromUi: fromUiHCutFreq,
    },
    95: {
        rig_ui_map: slope_map,
    },
    96: {
        rig_ui_map: {
            '0': 'SHIFT',
            '1': 'DTR',
            '2': 'DTR',
        },
    },
    97: {
        rig_ui_map: normal_reverse_map,
    },
    98: {
        rig_ui_map: normal_reverse_map,
    },
    99: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    100: {
        rig_ui_map: {
            '0': '170 Hz',
            '1': '200 Hz',
            '2': '425 Hz',
            '3': '850 Hz',
        },
    },
    101: {
        rig_ui_map: {
            '0': '1275 Hz',
            '1': '2125 Hz',
        },
    },
    102: {
        toUi: toUiLCutFreq,
        fromUi: fromUiLCutFreq,
    },
    103: {
        rig_ui_map: slope_map,
    },
    104: {
        toUi: toUiHCutFreq,
        fromUi: fromUiHCutFreq,
    },
    105: {
        rig_ui_map: slope_map,
    },
    106: {
        rig_ui_map: mic_select_map,
    },
    107: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    108: {
        rig_ui_map: ptt_select_map,
    },
    109: {
        rig_ui_map: port_select_map,
    },
    110: {
        rig_ui_map: {
            '0': '50 ~ 3000',
            '1': '100 ~ 2900',
            '2': '200 ~ 2800',
            '3': '300 ~ 2700',
            '4': '400 ~ 2600',
        },
    },
    111: {
        rig_ui_map: {
            '0': 'NARROW',
            '1': 'MEDIUM',
            '2': 'WIDE',
        },
    },
    112: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    113: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    114: {
        rig_ui_map: {
            '0': 'NARROW',
            '1': 'WIDE',
        },
    },
    115: {
        rig_ui_map: {
            '0': 'SPECTRUM',
            '1': 'WATER FALL',
        },
    },
    116: {
        rig_ui_map: {
            '03': '50 kHx',
            '04': '100 kHx',
            '05': '200 kHx',
            '06': '500 kHx',
            '07': '1000 kHx',
        },
    },
    117: {
        rig_ui_map: standard_colors_map,
    },
    118: {
        rig_ui_map: {
            '0': 'BLUE',
            '1': 'GRAY',
            '2': 'GREEN',
            '3': 'ORANGE',
            '4': 'PURPLE',
            '5': 'RED',
            '6': 'SKY BLUE',
            '7': 'MULTI',
        },
    },
    119: {
        rig_ui_map: prmtrc_eq1_freq_map,
    },
    120: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    121: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    122: {
        rig_ui_map: prmtrc_eq2_freq_map,
    },
    123: {
        toUi: toUiInt,
        fromUi: defaultFromUi,
    },
    124: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    125: {
        rig_ui_map: prmtrc_eq3_freq_map,
    },
    126: {
        toUi: toUiInt,
        fromUi: defaultFromUi,
    },
    127: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    128: {
        rig_ui_map: prmtrc_eq1_freq_map,
    },
    129: {
        toUi: toUiInt,
        fromUi: defaultFromUi,
    },
    130: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    131: {
        rig_ui_map: prmtrc_eq2_freq_map,
    },
    132: {
        toUi: toUiInt,
        fromUi: defaultFromUi,
    },
    133: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    134: {
        rig_ui_map: prmtrc_eq3_freq_map,
    },
    135: {
        toUi: toUiInt,
        fromUi: defaultFromUi,
    },
    136: {
        toUi: toUiInt,
        fromUi: pad2,
    },
    137: {
        toUi: toUiInt,
        fromUi: fromUiMaxPower(100),
    },
    138: {
        toUi: toUiInt,
        fromUi: fromUiMaxPower(100),
    },
    139: {
        toUi: toUiInt,
        fromUi: fromUiMaxPower(50),
    },
    140: {
        toUi: toUiInt,
        fromUi: fromUiMaxPower(50),
    },
    141: {
        rig_ui_map: {
            '0': 'OFF',
            '1': 'INTERNAL',
            '2': 'EXTERNAL',
            '3': 'ATAS',
            '4': 'LAMP',
        },
    },
    142: {
        rig_ui_map: {
            '0': 'MIC',
            '1': 'DATA',
        },
    },
    143: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    144: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    145: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    146: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    147: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    148: {
        toUi: toUiInt,
        fromUi: pad3,
    },
    149: {
        rig_ui_map: enable_disable_map,
    },
    150: {
        rig_ui_map: {
            '0': 'MANUAL',
            '1': 'PRESET',
        },
    },
    151: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
    152: {
        rig_ui_map: {
            '0': 'HISTORY',
            '1': 'ACTIVITY',
        },
    },
    153: {
        toUi: defaultToUi,
        fromUi: defaultFromUi,
    },
};

class rig_menu {
    static toUi(no, value) {
        const mi = mi_table[no];

        if(isEmptyValue(mi)) {
            throw new Error('rig_menu.toUi(' + no + ') not found');
        }

        if (isEmptyValue(value)) {
            throw new Error('rig_menu.toUi(' + no + ') has no value');
        }

        if(mi.hasOwnProperty('rig_ui_map')) {
            if(mi.rig_ui_map.hasOwnProperty(value)) {
                return mi.rig_ui_map[value];
            }
        }
        else if(mi.hasOwnProperty('toUi')) {
            return mi.toUi(value);
        }
        throw new Error('rig_menu.toUi(' + no + '): unknown value: "' + value + '"');
    }

    static fromUi(no, value) {
        const mi = mi_table[no];

        if(isEmptyValue(mi)) {
            throw new Error('rig_menu.fromUi(' + no + ') not found');
        }

        if (value !== null) {
            if(mi.hasOwnProperty('rig_ui_map')) {
                if(!mi.hasOwnProperty('ui_rig_map')) {
                    mi.ui_rig_map = {};

                    for(const [k,v] of Object.entries(mi.rig_ui_map)) {
                        mi.ui_rig_map[v] = k;
                    }
                }

                if(mi.ui_rig_map.hasOwnProperty(value)) {
                    return mi.ui_rig_map[value];
                }
            }
            else if(mi.hasOwnProperty('fromUi')) {
                return mi.fromUi(value);
            }
            throw new Error('rig_menu.fromUi(' + no + '): unknown value: "' + value + '"');
        }

        throw new Error('rig_menu.fromUi(' + no + ') has no value');
    }
}

export { rig_menu };
