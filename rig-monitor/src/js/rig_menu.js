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
const pad5 = pad(5);
const pad8 = pad(8);
const pad9 = pad(9);

function pad_w_sign(len) {
    return function (value) {
        if (isEmptyValue(value)) {
            return null;
        }

        let sign = '+';
        if (value < 0) {
            sign = '-';
            value = -value;
        }

        const f_pad = pad(len - 1);
        return sign + f_pad(Math.floor(value));
    };
}

const pad3_w_sign = pad_w_sign(3);
const pad4_w_sign = pad_w_sign(4);
const pad5_w_sign = pad_w_sign(5);

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

    return pad4(value - (value % 20));
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

function sprintf(format) {
  let args = Array.prototype.slice.call(arguments, 1);
  let i = 0;

  return format.replace(/%([+\-\#0 ]*)(\d+)?(\.\d+)?([b-fouxX])/g, function(match, flags, width, precision, specifier) {
    let arg = args[i++];
    let str = '';
    
    switch (specifier) {
      case 'b':
        str = Number(arg).toString(2);
        break;
      case 'c':
        str = String.fromCharCode(Number(arg));
        break;
      case 'd':
      case 'i':
        str = Number(arg).toFixed(0);
        break;
      case 'f':
        str = Number(arg).toFixed(precision ? parseInt(precision.slice(1)) : 6);
        break;
      case 'o':
        str = Number(arg).toString(8);
        break;
      case 'u':
        str = Math.abs(Number(arg)).toFixed(0);
        break;
      case 'x':
        str = Number(arg).toString(16).toLowerCase();
        break;
      case 'X':
          str = Number(arg).toString(16).toUpperCase();
          break;
    }

    if (flags.includes('+') && Number(arg) >= 0) {
        str = '+' + str;
    } else if (flags.includes('-') && Number(arg) < 0) {
        str = '-' + str;
    } else if (flags.includes(' ') && Number(arg) >= 0) {
        str = ' ' + str
    }
    
    if (width) {
        let padding = ' ';
        if (flags.includes('0')) {
            padding = '0';
        }
        while (str.length < width) {
            if (flags.includes('-'))
                str = str + padding
            else
                str = padding + str;
        }
    }
    return str;
  });
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


const prmtrc_eq1_freq_map = {'00': 'OFF',};
for (let i = 1; i <= 7; i++) {
    prmtrc_eq1_freq_map[pad2(i)] = `${i * 100} Hz`;
}

const prmtrc_eq2_freq_map = {'00': 'OFF',};
for (let i = 1; i <= 9; i++) {
    prmtrc_eq2_freq_map[pad2(i)] = `${(i + 6) * 100} Hz`;
}

const prmtrc_eq3_freq_map = {'00': 'OFF',};
for (let i = 1; i <= 18; i++) {
    prmtrc_eq3_freq_map[pad2(i)] = `${i * 100 + 1400} Hz`;
}

const lcut_freq_map = {'00': 'OFF',};
for (let i = 1; i <= 19; i++) {
    lcut_freq_map[pad2(i)] = `${i * 50 + 50} Hz`;
}

const hcut_freq_map = {
    '00': 'OFF',
};
for (let i = 1; i <= 67; i++) {
    hcut_freq_map[pad2(i)] = `${i * 50 + 650} Hz`;
}

const tx_tot_map = {'00': 'OFF'};
for (let i = 1; i <= 30; i++) {
    tx_tot_map[pad2(i)] = `${i} sec`;
}

// End of maps

const mi_table = {
    1: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
	    range: {min:'20', max:'4000', step:'20'},
        suffix: ' ms',
    },
    2: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
	    range: {min:'20', max:'4000', step:'20'},
        suffix: ' ms',
    },
    3: {
        toUi: toUiInt,
        fromUi: fromUiIntBy20,
	    range: {min:'20', max:'4000', step:'20'},
        suffix: ' ms',
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
        rig_ui_map: {
            '0': 'OFF',
            '1': '1 sec',
            '2': '2 sec',
            '3': '3 sec',
            '4': '4 sec',
            '5': '5 sec',
        },
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
        range: {min:'0', max:'15'},
    },
    9: {
        rig_ui_map: {
            '0': 'OFF',
            '1': '0.5 sec',
            '2': '1.0 sec',
            '3': '2.0 sec',
        },
    },
    10: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
    },
    11: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
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
        sorted_list: null,
        rig_ui_map: {
            '25': '2.5', '26': '2.6', '27': '2.7', '28': '2.8', '29': '2.9',
            '30': '3.0', '31': '3.1', '32': '3.2', '33': '3.3', '34': '3.4',
            '35': '3.5', '36': '3.6', '37': '3.7', '38': '3.8', '39': '3.9',
            '40': '4.0', '41': '4.1', '42': '4.2', '43': '4.3', '44': '4.4',
            '45': '4.5',
        },
    },
    15: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {min:'0', max:'698'},
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
        range: {min:'0', max:'9999'},
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
        range: {min:'0', max:'10'},
    },
    26: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
    },
    27: {
        rig_ui_map: {
            '-1200': '-12:00', '-1130': '-11:30', '-1100': '-11:00',
            '-1030': '-10:30', '-1000': '-10:00', '-0930': '-09:30',
            '-0900': '-09:00', '-0830': '-08:30', '-0800': '-08:00',
            '-0730': '-07:30', '-0700': '-07:00', '-0630': '-06:30',
            '-0600': '-06:00', '-0530': '-05:30', '-0500': '-05:00',
            '-0430': '-04:30', '-0400': '-04:00', '-0330': '-03:30',
            '-0300': '-03:00', '-0230': '-02:30', '-0200': '-02:00',
            '-0130': '-01:30', '-0100': '-01:00', '-0030': '-00:30',
            '+0000': '00:00', '+0030': '+00:30', '+0100': '+01:00',
            '+0130': '+01:30', '+0200': '+02:00', '+0230': '+02:30',
            '+0300': '+03:00', '+0330': '+03:30', '+0400': '+04:00',
            '+0430': '+04:30', '+0500': '+05:00', '+0530': '+05:30',
            '+0600': '+06:00', '+0630': '+06:30', '+0700': '+07:00',
            '+0730': '+07:30', '+0800': '+08:00', '+0830': '+08:30',
            '+0900': '+09:00', '+0930': '+09:30', '+1000': '+10:00',
            '+1030': '+10:30', '+1100': '+11:00', '+1130': '+11:30',
            '+1200': '+12:00', '+1230': '+12:30', '+1300': '+13:00',
            '+1330': '+13:30', '+1400': '+14:00',
        },
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
        rig_ui_map: {
            '-20': '-20 kHz', '-19': '-19 kHz', '-18': '-18 kHz',
            '-17': '-17 kHz', '-16': '-16 kHz', '-15': '-15 kHz',
            '-14': '-14 kHz', '-13': '-13 kHz', '-12': '-12 kHz',
            '-11': '-11 kHz', '-10': '-10 kHz', '-09': '-9 kHz',
            '-08': '-8 kHz', '-07': '-7 kHz', '-06': '-6 kHz',
            '-05': '-5 kHz', '-04': '-4 kHz', '-03': '-3 kHz',
            '-02': '-2 kHz', '-01': '-1 kHz', '-00': '0 kHz',
            '+00': '0 kHz', '+01': '1 kHz', '+02': '2 kHz',
            '+03': '3 kHz', '+04': '4 kHz', '+05': '5 kHz',
            '+06': '6 kHz', '+07': '7 kHz', '+08': '8 kHz',
            '+09': '9 kHz', '+10': '10 kHz', '+11': '11 kHz',
            '+12': '12 kHz', '+13': '13 kHz', '+14': '14 kHz',
            '+15': '15 kHz', '+16': '16 kHz', '+17': '17 kHz',
            '+18': '18 kHz', '+19': '19 kHz', '+20': '20 kHz',
        },
    },
    36: {
        sorted_list: null,
        rig_ui_map: tx_tot_map,
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
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-25', max:'25'},
    },
    40: {
        rig_ui_map: {
            '0': 'RX',
            '1': 'TX',
            '2': 'TRX',
        },
    },
    41: {
        sorted_list: null,
        rig_ui_map: lcut_freq_map,
    },
    42: {
        rig_ui_map: slope_map,
    },
    43: {
        sorted_list: null,
        rig_ui_map: hcut_freq_map,
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
        range: {},
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
        range: {},
    },
    50: {
        sorted_list: null,
        rig_ui_map: lcut_freq_map,
    },
    51: {
        rig_ui_map: slope_map,
    },
    52: {
        sorted_list: null,
        rig_ui_map: hcut_freq_map,
    },
    53: {
        rig_ui_map: slope_map,
    },
    54: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
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
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'300', max:'3000', step:'10'},
        suffix: ' ms',
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
        toUi: toUiInt,
        fromUi: pad5_w_sign,
        range: {min:'-3000', max:'3000', step:'10'},
    },
    65: {
        toUi: toUiInt,
        fromUi: pad5_w_sign,
        range: {min:'-3000', max:'3000', step:'10'},
    },
    66: {
        sorted_list: null,
        rig_ui_map: lcut_freq_map,
    },
    67: {
        rig_ui_map: slope_map,
    },
    68: {
        sorted_list: null,
        rig_ui_map: hcut_freq_map,
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
        range: {},
    },
    74: {
        rig_ui_map: mic_select_map,
    },
    75: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
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
        range: {},
    },
    79: {
        rig_ui_map: {
            '0': '1200',
            '1': '9600',
        },
    },
    80: {
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'0', max:'1000', step:'10'},
    },
    81: {
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'0', max:'4000', step:'10'},
    },
    82: {
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'0', max:'4000', step:'10'},
    },
    83: {
        toUi: toUiInt,
        fromUi: pad5,
        range: {min:'0', max:'10000', step:'10'},
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
        sorted_list: null,
        rig_ui_map: lcut_freq_map,
    },
    93: {
        rig_ui_map: slope_map,
    },
    94: {
        sorted_list: null,
        rig_ui_map: hcut_freq_map,
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
        range: {},
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
        sorted_list: null,
        rig_ui_map: lcut_freq_map,
    },
    103: {
        rig_ui_map: slope_map,
    },
    104: {
        sorted_list: null,
        rig_ui_map: hcut_freq_map,
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
        range: {},
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
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-40', max:'20'},
    },
    113: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {max: '11'},
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
            '03': '50 kHz',
            '04': '100 kHz',
            '05': '200 kHz',
            '06': '500 kHz',
            '07': '1000 kHz',
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
        sorted_list: null,
        rig_ui_map: prmtrc_eq1_freq_map,
    },
    120: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    121: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
    },
    122: {
        sorted_list: null,
        rig_ui_map: prmtrc_eq2_freq_map,
    },
    123: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    124: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
    },
    125: {
        sorted_list: null,
        rig_ui_map: prmtrc_eq3_freq_map,
    },
    126: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    127: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
    },
    128: {
        rig_ui_map: prmtrc_eq1_freq_map,
    },
    129: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    130: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
    },
    131: {
        sorted_list: null,
        rig_ui_map: prmtrc_eq2_freq_map,
    },
    132: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    133: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
    },
    134: {
        sorted_list: null,
        rig_ui_map: prmtrc_eq3_freq_map,
    },
    135: {
        toUi: toUiInt,
        fromUi: pad3_w_sign,
        range: {min:'-20', max:'10'},
    },
    136: {
        toUi: toUiInt,
        fromUi: pad2,
        range: {min: '1', max: '10'},
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
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'30', max:'3000', step:'10'},
        suffix: ' ms',
    },
    145: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
    },
    146: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
    },
    147: {
        toUi: toUiInt,
        fromUi: pad4,
        range: {min:'30', max:'3000'},
        suffix: ' ms',
    },
    148: {
        toUi: toUiInt,
        fromUi: pad3,
        range: {},
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
        toUi: toUiInt,
        fromUi: pad8,
        range: {min:'00030000', max:'47000000'},
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

function to_ui(no, value) {
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

function from_ui(no, value) {
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

function menu_list(no) {
    const mi = mi_table[no];

    if (mi.hasOwnProperty('sorted_list')) {
        if (mi.sorted_list === null) {
            mi.sorted_list = Object.keys(mi.rig_ui_map)
                .sort((keyA, keyB) => keyA.localeCompare(keyB))
                .map(key => (mi.rig_ui_map[key] ));
        }
        return mi.sorted_list;
    }

    if(mi.hasOwnProperty('rig_ui_map')) {
        return Object.keys(mi.rig_ui_map).map(k => ( mi.rig_ui_map[k] ));
    }
    return null;
}

function menu_range(no) {
    const mi = mi_table[no];

    if(mi.hasOwnProperty('range')) {
        return mi.range;
    }
    return null;
}

function menu_suffix(no) {
    const mi = mi_table[no];

    if(mi.hasOwnProperty('suffix')) {
        return mi.suffix;
    }
    return null;
}

export { to_ui, from_ui, menu_list, menu_range, menu_suffix };
