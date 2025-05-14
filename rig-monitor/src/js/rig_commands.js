
const rig_commands = {
    settings: {
        af_gain: { desc: 'AF_GAIN', value: null, changed: false, fav: false, saved: false},
        agc: {desc: 'AGC', value: null, changed: false, fav: false, saved: false},
        tuner: { desc: 'ANTENNA_TUNER_CONTROL', value: null, changed: false, fav: false, saved: false},
        apf_freq: { desc: 'APF_FREQ', value: null, changed: false, fav: false, saved: false},
        apf: { desc: 'APF_ON/OFF', value: null, changed: false, fav: false, saved: false},
        auto_notch: { desc: 'AUTO_NOTCH', value: null, changed: false, fav: false, saved: false},
        break_in: { desc: 'BREAK-IN', value: null, changed: false, fav: false, saved: false},
        clar: { desc: 'CLAR', value: null, changed: false, fav: false, saved: false},
        contour_freq: { desc: 'CONTOUR_FREQ', value: null, changed: false, fav: false, saved: false},
        contour_on: { desc: 'CONTOUR_ON/OFF', value: null, changed: false, fav: false, saved: false},
        ctcss: { desc: 'CTCSS', value: null, changed: false, fav: false, saved: false},
        ctcss_tone_frequency: { desc: 'CTCSS_TONE_FREQUENCY', value: null, changed: false, fav: false, saved: false},
        cw_break_in_delay_time: { desc: 'CW_BREAK-IN_DELAY_TIME', value: null, changed: false, fav: false, saved: false},
        cw_spot: { desc: 'CW_SPOT', value: null, changed: false, fav: false, saved: false},
        date: { desc: 'DATE', value: null, changed: false, fav: false, saved: false},
        dcs_tone_frequency: { desc: 'DCS_TONE_FREQUENCY', value: null, changed: false, fav: false, saved: false},
        fast_step: { desc: 'FAST_STEP', value: null, changed: false, fav: false, saved: false},
        vfo_a: { desc: 'FREQUENCY_VFO-A', value: null, changed: false, fav: false, saved: false},
        vfo_b: { desc: 'FREQUENCY_VFO-B', value: null, changed: false, fav: false, saved: false},
        if_shift: { desc: 'IF-SHIFT', value: null, changed: false, fav: false, saved: false},
        key_pitch: { desc: 'KEY_PITCH', value: null, changed: false, fav: false, saved: false},
        key_speed: { desc: 'KEY_SPEED', value: null, changed: false, fav: false, saved: false},
        keyer: { desc: 'KEYER', value: null, changed: false, fav: false, saved: false},
        lock: { desc: 'LOCK', value: null, changed: false, fav: false, saved: false},
        manual_notch_level: { desc: 'MANUAL_NOTCH_LEVEL', value: null, changed: false, fav: false, saved: false},
        manual_notch_on: { desc: 'MANUAL_NOTCH_ON/OFF', value: null, changed: false, fav: false, saved: false},
        meter_sw: { desc: 'METER_SW', value: null, changed: false, fav: false, saved: false},
        mic_gain: { desc: 'MIC_GAIN', value: null, changed: false, fav: false, saved: false},
        mode: { desc: 'MODE', value: null, changed: false, fav: false, saved: false},
        monitor_level: { desc: 'MONITOR_LEVEL', value: null, changed: false, fav: false, saved: false},
        monitor_on: { desc: 'MONITOR_ON/OFF', value: null, changed: false, fav: false, saved: false},
        mox_set: { desc: 'MOX_SET', value: null, changed: false, fav: false, saved: false},
        narrow: { desc: 'NARROW', value: null, changed: false, fav: false, saved: false},
        noise_blanker_level: { desc: 'NOISE_BLANKER_LEVEL', value: null, changed: false, fav: false, saved: false},
        noise_blanker_status: { desc: 'NOISE_BLANKER_STATUS', value: null, changed: false, fav: false, saved: false},
        noise_reduction: { desc: 'NOISE_REDUCTION', value: null, changed: false, fav: false, saved: false},
        noise_reduction_level: { desc: 'NOISE_REDUCTION_LEVEL', value: null, changed: false, fav: false, saved: false},
        offset: { desc: 'OFFSET_(REPEATER_SHIFT)', value: null, changed: false, fav: false, saved: false},
        parametric_microphone_equalizer: { desc: 'PARAMETRIC_MICROPHONE_EQUALIZER', value: null, changed: false, fav: false, saved: false},
        pll_unlock_status: { desc: 'PLL_UNLOCK_STATUS', value: null, changed: false, fav: false, saved: false},
        power_level: { desc: 'POWER_CONTROL', value: null, changed: false, fav: false, saved: false},
        pre_amp: { desc: 'PRE-AMP_(IPO)', value: null, changed: false, fav: false, saved: false},
        radio_status: { desc: 'RADIO_STATUS', value: null, changed: false, fav: false, saved: false},
        rf_attenuator: { desc: 'RF_ATTENUATOR', value: null, changed: false, fav: false, saved: false},
        rf_gain: { desc: 'RF_GAIN', value: null, changed: false, fav: false, saved: false},
        speech_processor_level: { desc: 'SPEECH_PROCESSOR_LEVEL', value: null, changed: false, fav: false, saved: false},
        speech_processor_on: { desc: 'SPEECH_PROCESSOR_ON/OFF', value: null, changed: false, fav: false, saved: false},
        squelclh_level: { desc: 'SQUELCLH_LEVEL', value: null, changed: false, fav: false, saved: false},
        time: { desc: 'TIME', value: null, changed: false, fav: false, saved: false},
        time_differential: { desc: 'TIME_DIFFERENTIAL', value: null, changed: false, fav: false, saved: false},
        transmit: { desc: 'TX_SET', value: null, changed: false, fav: false, saved: false},
        tx_clar: { desc: 'TX_CLAR', value: null, changed: false, fav: false, saved: false},
        txw: { desc: 'TXW', value: null, changed: false, fav: false, saved: false},
        vox_gain: { desc: 'VOX_GAIN', value: null, changed: false, fav: false, saved: false},
        vox_status: { desc: 'VOX_STATUS', value: null, changed: false, fav: false, saved: false},
    },
    menu_items: {
        1:   {desc: 'AGC_FAST_DELAY', value: null, changed: false, fav: false, saved: false},
        2:   {desc: 'AGC_MID_DELAY', value: null, changed: false, fav: false, saved: false},
        3:   {desc: 'AGC_SLOW_DELAY', value: null, changed: false, fav: false, saved: false},
        4:   {desc: 'HOME_FUNCTION', value: null, changed: false, fav: false, saved: false},
        5:   {desc: 'MY_CALL_INDICATION', value: null, changed: false, fav: false, saved: false},
        6:   {desc: 'DISPLAY_COLOR', value: null, changed: false, fav: false, saved: false},
        7:   {desc: 'DIMMER_LED', value: null, changed: false, fav: false, saved: false},
        8:   {desc: 'DIMMER_TFT', value: null, changed: false, fav: false, saved: false},
        9:   {desc: 'BAR_MTR_PEAK_HOLD', value: null, changed: false, fav: false, saved: false},
        10:  {desc: 'DVS_RX_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        11:  {desc: 'DVS_TX_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        12:  {desc: 'KEYER_TYPE', value: null, changed: false, fav: false, saved: false},
        13:  {desc: 'KEYER_DOT/DASH', value: null, changed: false, fav: false, saved: false},
        14:  {desc: 'CW_WEIGHT', value: null, changed: false, fav: false, saved: false},
        15:  {desc: 'BEACON_INTERVAL', value: null, changed: false, fav: false, saved: false},
        16:  {desc: 'NUMBER_STYLE', value: null, changed: false, fav: false, saved: false},
        17:  {desc: 'CONTEST_NUMBER', value: null, changed: false, fav: false, saved: false},
        18:  {desc: 'CW_MEMORY_1', value: null, changed: false, fav: false, saved: false},
        19:  {desc: 'CW_MEMORY_2', value: null, changed: false, fav: false, saved: false},
        20:  {desc: 'CW_MEMORY_3', value: null, changed: false, fav: false, saved: false},
        21:  {desc: 'CW_MEMORY_4', value: null, changed: false, fav: false, saved: false},
        22:  {desc: 'CW_MEMORY_5', value: null, changed: false, fav: false, saved: false},
        23:  {desc: 'NB_WIDTH', value: null, changed: false, fav: false, saved: false},
        24:  {desc: 'NB_REJECTION', value: null, changed: false, fav: false, saved: false},
        25:  {desc: 'NB_LEVEL', value: null, changed: false, fav: false, saved: false},
        26:  {desc: 'BEEP_LEVEL', value: null, changed: false, fav: false, saved: false},
        27:  {desc: 'TIME_ZONE', value: null, changed: false, fav: false, saved: false},
        28:  {desc: 'GPS/232C_SELECT', value: null, changed: false, fav: false, saved: false},
        29:  {desc: '232C_RATE', value: null, changed: false, fav: false, saved: false},
        30:  {desc: '232C_TOT', value: null, changed: false, fav: false, saved: false},
        31:  {desc: 'CAT_RATE', value: null, changed: false, fav: false, saved: false},
        32:  {desc: 'CAT_TOT', value: null, changed: false, fav: false, saved: false},
        33:  {desc: 'CAT_RTS', value: null, changed: false, fav: false, saved: false},
        34:  {desc: 'MEM_GROUP', value: null, changed: false, fav: false, saved: false},
        35:  {desc: 'QUICK_SPLIT_FREQ', value: null, changed: false, fav: false, saved: false},
        36:  {desc: 'TX_TOT', value: null, changed: false, fav: false, saved: false},
        37:  {desc: 'MIC_SCAN', value: null, changed: false, fav: false, saved: false},
        38:  {desc: 'MIC_SCAN_RESUME', value: null, changed: false, fav: false, saved: false},
        39:  {desc: 'REF_FREQ_ADJ', value: null, changed: false, fav: false, saved: false},
        40:  {desc: 'CLAR_MODE_SELECT', value: null, changed: false, fav: false, saved: false},
        41:  {desc: 'AM_LCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        42:  {desc: 'AM_LCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        43:  {desc: 'AM_HCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        44:  {desc: 'AM_HCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        45:  {desc: 'AM_MIC_SELECT', value: null, changed: false, fav: false, saved: false},
        46:  {desc: 'AM_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        47:  {desc: 'AM_PTT_SELECT', value: null, changed: false, fav: false, saved: false},
        48:  {desc: 'AM_PORT_SELECT', value: null, changed: false, fav: false, saved: false},
        49:  {desc: 'AM_DATA_GAIN', value: null, changed: false, fav: false, saved: false},
        50:  {desc: 'CW_LCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        51:  {desc: 'CW_LCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        52:  {desc: 'CW_HCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        53:  {desc: 'CW_HCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        54:  {desc: 'CW_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        55:  {desc: 'CW_AUTO_MODE', value: null, changed: false, fav: false, saved: false},
        56:  {desc: 'CW_BK-IN_TYPE', value: null, changed: false, fav: false, saved: false},
        57:  {desc: 'CW_BK-IN_DELAY', value: null, changed: false, fav: false, saved: false},
        58:  {desc: 'CW_WAVE_SHAPE', value: null, changed: false, fav: false, saved: false},
        59:  {desc: 'CW_FREQ_DISPLAY', value: null, changed: false, fav: false, saved: false},
        60:  {desc: 'PC_KEYING', value: null, changed: false, fav: false, saved: false},
        61:  {desc: 'QSK_DELAY_TIME', value: null, changed: false, fav: false, saved: false},
        62:  {desc: 'DATA_MODE', value: null, changed: false, fav: false, saved: false},
        63:  {desc: 'PSK_TONE', value: null , changed: false, fav: false, saved: false},
        64:  {desc: 'OTHER_DISP_(SSB)', value: null, changed: false, fav: false, saved: false},
        65:  {desc: 'OTHER_SHIFT_(SSB)', value: null, changed: false, fav: false, saved: false},
        66:  {desc: 'DATA_LCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        67:  {desc: 'DATA_LCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        68:  {desc: 'DATA_HCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        69:  {desc: 'DATA_HCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        70:  {desc: 'DATA_IN_SELECT', value: null , changed: false, fav: false, saved: false},
        71:  {desc: 'DATA_PTT_SELECT', value: null , changed: false, fav: false, saved: false},
        72:  {desc: 'DATA_PORT_SELECT', value: null , changed: false, fav: false, saved: false},
        73:  {desc: 'DATA_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        74:  {desc: 'FM_MIC_SELECT', value: null, changed: false, fav: false, saved: false},
        75:  {desc: 'FM_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        76:  {desc: 'FM_PKT_PTT_SELECT', value: null, changed: false, fav: false, saved: false},
        77:  {desc: 'FM_PKT_PORT_SELECT', value: null, changed: false, fav: false, saved: false},
        78:  {desc: 'FM_PKT_TX_GAIN', value: null, changed: false, fav: false, saved: false},
        79:  {desc: 'FM_PKT_MODE', value: null, changed: false, fav: false, saved: false},
        80:  {desc: 'RPT_SHIFT_28MHz', value: null, changed: false, fav: false, saved: false},
        81:  {desc: 'RPT_SHIFT_50MHz', value: null, changed: false, fav: false, saved: false},
        82:  {desc: 'RPT_SHIFT_144MHz', value: null, changed: false, fav: false, saved: false},
        83:  {desc: 'RPT_SHIFT_430MHz', value: null, changed: false, fav: false, saved: false},
        84:  {desc: 'ARS_144MHz', value: null, changed: false, fav: false, saved: false},
        85:  {desc: 'ARS_430MHz', value: null, changed: false, fav: false, saved: false},
        86:  {desc: 'DCS_POLARITY', value: null, changed: false, fav: false, saved: false},
        87:  {desc: 'RADIO_ID', value: null, changed: false, fav: false, saved: false},
        88:  {desc: 'GM_DISPLY', value: null, changed: false, fav: false, saved: false},
        89:  {desc: 'DISTANCE', value: null, changed: false, fav: false, saved: false},
        90:  {desc: 'AMS_TX_MODE', value: null, changed: false, fav: false, saved: false},
        91:  {desc: 'STANDBY_BEEP', value: null, changed: false, fav: false, saved: false},
        92:  {desc: 'RTTY_LCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        93:  {desc: 'RTTY_LCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        94:  {desc: 'RTTY_HCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        95:  {desc: 'RTTY_HCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        96:  {desc: 'RTTY_SHIFT_PORT', value: null, changed: false, fav: false, saved: false},
        97:  {desc: 'RTTY_POLARITY-RX', value: null, changed: false, fav: false, saved: false},
        98:  {desc: 'RTTY_POLARITY-TX', value: null, changed: false, fav: false, saved: false},
        99:  {desc: 'RTTY_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        100: {desc: 'RTTY_SHIFT_FREQ', value: null, changed: false, fav: false, saved: false},
        101: {desc: 'RTTY_MARK_FREQ', value: null, changed: false, fav: false, saved: false},
        102: {desc: 'SSB_LCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        103: {desc: 'SSB_LCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        104: {desc: 'SSB_HCUT_FREQ', value: null, changed: false, fav: false, saved: false},
        105: {desc: 'SSB_HCUT_SLOPE', value: null, changed: false, fav: false, saved: false},
        106: {desc: 'SSB_MIC_SELECT', value: null, changed: false, fav: false, saved: false},
        107: {desc: 'SSB_OUT_LEVEL', value: null, changed: false, fav: false, saved: false},
        108: {desc: 'SSB_PTT_SELECT', value: null, changed: false, fav: false, saved: false},
        109: {desc: 'SSB_PORT_SELECT', value: null, changed: false, fav: false, saved: false},
        110: {desc: 'SSB_TX_BPF', value: null, changed: false, fav: false, saved: false},
        111: {desc: 'APF_WIDTH', value: null, changed: false, fav: false, saved: false},
        112: {desc: 'CONTOUR_LEVEL', value: null, changed: false, fav: false, saved: false},
        113: {desc: 'CONTOUR_WIDTH', value: null, changed: false, fav: false, saved: false},
        114: {desc: 'IF_NOTCH_WIDTH', value: null, changed: false, fav: false, saved: false},
        115: {desc: 'SCP_DISPLAY_MODE', value: null, changed: false, fav: false, saved: false},
        116: {desc: 'SCP_SPAN_FREQ', value: null, changed: false, fav: false, saved: false},
        117: {desc: 'SPECTRUM_COLOR', value: null, changed: false, fav: false, saved: false},
        118: {desc: 'WATER_FALL_COLOR', value: null, changed: false, fav: false, saved: false},
        119: {desc: 'PRMTRC_EQ1_FREQ', value: null, changed: false, fav: false, saved: false},
        120: {desc: 'PRMTRC_EQ1_LEVEL', value: null, changed: false, fav: false, saved: false},
        121: {desc: 'PRMTRC_EQ1_BWTH', value: null, changed: false, fav: false, saved: false},
        122: {desc: 'PRMTRC_EQ2_FREQ', value: null, changed: false, fav: false, saved: false},
        123: {desc: 'PRMTRC_EQ2_LEVEL', value: null, changed: false, fav: false, saved: false},
        124: {desc: 'PRMTRC_EQ2_BWTH', value: null, changed: false, fav: false, saved: false},
        125: {desc: 'PRMTRC_EQ3_FREQ', value: null, changed: false, fav: false, saved: false},
        126: {desc: 'PRMTRC_EQ3_LEVEL', value: null, changed: false, fav: false, saved: false},
        127: {desc: 'PRMTRC_EQ3_BWTH', value: null, changed: false, fav: false, saved: false},
        128: {desc: 'P-PRMTRC_EQ1_FREQ', value: null, changed: false, fav: false, saved: false},
        129: {desc: 'P-PRMTRC_EQ1_LEVEL', value: null, changed: false, fav: false, saved: false},
        130: {desc: 'P-PRMTRC_EQ1_BWTH', value: null, changed: false, fav: false, saved: false},
        131: {desc: 'P-PRMTRC_EQ2_FREQ', value: null, changed: false, fav: false, saved: false},
        132: {desc: 'P-PRMTRC_EQ2_LEVEL', value: null, changed: false, fav: false, saved: false},
        133: {desc: 'P-PRMTRC_EQ2_BWTH', value: null, changed: false, fav: false, saved: false},
        134: {desc: 'P-PRMTRC_EQ3_FREQ', value: null, changed: false, fav: false, saved: false},
        135: {desc: 'P-PRMTRC_EQ3_LEVEL', value: null, changed: false, fav: false, saved: false},
        136: {desc: 'P-PRMTRC_EQ3_BWTH', value: null, changed: false, fav: false, saved: false},
        137: {desc: 'HF_TX_MAX_POWER', value: null, changed: false, fav: false, saved: false},
        138: {desc: '50M_TX_MAX_POWER', value: null, changed: false, fav: false, saved: false},
        139: {desc: '144M_TX_MAX_POWER', value: null, changed: false, fav: false, saved: false},
        140: {desc: '430M_TX_MAX_POWER', value: null, changed: false, fav: false, saved: false},
        141: {desc: 'TUNER_SELECT', value: null, changed: false, fav: false, saved: false},
        142: {desc: 'VOX_SELECT', value: null, changed: false, fav: false, saved: false},
        143: {desc: 'VOX_GAIN', value: null, changed: false, fav: false, saved: false},
        144: {desc: 'VOX_DELAY', value: null, changed: false, fav: false, saved: false},
        145: {desc: 'ANTI_VOX_GAIN', value: null, changed: false, fav: false, saved: false},
        146: {desc: 'DATA_VOX_GAIN', value: null, changed: false, fav: false, saved: false},
        147: {desc: 'DATA_VOX_DELAY', value: null, changed: false, fav: false, saved: false},
        148: {desc: 'ANTI_DVOX_GAIN', value: null, changed: false, fav: false, saved: false},
        149: {desc: 'EMERGENCY_FREQ_TX', value: null, changed: false, fav: false, saved: false},
        150: {desc: 'PRT/WIRES_FREQ', value: null, changed: false, fav: false, saved: false},
        151: {desc: 'PRESET_FREQUENCY', value: null, changed: false, fav: false, saved: false},
        152: {desc: 'SEARCH_SETUP', value: null, changed: false, fav: false, saved: false},
        153: {desc: 'WIRES_DG-ID', value: null, changed: false, fav: false, saved: false},
    },
    modeOptions: [
        'LSB', 'USB', 'FM', 'DATA-USB', 'DATA-FM', 'RTTY-LSB', 'RTTY-USB',
        'CW', 'CW-R', 'DATA-LSB', 'AM', 'AM-N', 'FM-N', 'C4FM'
    ],
    offsetOptions: ['Simplex','+','-'],
    squelchModeOptions: ['CTCSS OFF', 'CTCSS ENC/DEC', 'CTCSS ENC', 'DCS ENC/DEC', 'DCS ENC'],
    ctcssToneOptions: [
        '67.0', '69.3', '71.9', '74.4', '77.0', '79.7', '82.5', '85.4',
        '88.5', '91.5', '94.8', '97.4', '100.0', '103.5', '107.2',
        '110.9', '114.8', '118.8', '123.0', '127.3', '131.8', '136.5',
        '141.3', '146.2', '151.4', '156.7', '159.8', '162.2', '165.5',
        '167.9', '171.3', '173.8', '177.3', '179.9', '183.5', '186.2',
        '189.9', '192.8', '196.6', '199.5', '203.5', '206.5', '210.7',
        '218.1', '225.7', '229.1', '233.6', '241.8', '250.3', '254.1'
    ],
    dcsToneOptions: [
        '23',  '25',  '26',  '31',  '32',  '36',  '43',  '47',  '51',
        '53',  '54',  '65',  '71',  '72', '73',  '74', '114', '115',
        '116', '122', '125', '131', '132', '134', '143', '145', '152',
        '155', '156', '162', '165', '172', '174', '205', '212', '223',
        '225', '226', '243', '244', '245', '246', '251', '252', '255',
        '261', '263', '265', '266', '271', '274', '306', '311', '315',
        '325', '331', '332', '343', '346', '351', '356', '364', '365',
        '371', '411', '412', '413', '423', '431', '432', '445', '446',
        '452', '454', '455', '462', '464', '465', '466', '503', '506',
        '516', '523', '526', '532', '546', '565', '606', '612', '624',
        '627', '631', '632', '654', '662', '664', '703', '712', '723',
        '731', '732', '734', '743', '754'
    ],
    FreqUtils: (function () {
        function trim_zeros(value) {
            if(value.substring(value.length - 1) === '0') {
                return trim_zeros(value.substring(0, value.length - 1));
            }

            if(value.substring(value.length - 1) === '.') {
                return value.substring(0, value.length - 1);
            }

            return value;
        }

        function padTo(value, l) {
            if(value.length < l) {
                return padTo(value + '0', l);
            }

            if(value.length === l) {
                return value;
            }

            return value.substring(0, l);
        }

        function add_dec_frac(dec, frac) {
            if(dec < 1000) {
                return dec * 1000000 + frac;
            }

            if(dec < 1000000) {
                return dec * 1000 + frac;
            }

            return dec;
        }

        function scale_frac(dec, value) {
            if(dec < 1000) {
                return parseInt(padTo(value, 6), 10);
            }

            if(dec < 1000000) {
                return parseInt(padTo(value, 3), 10);
            }

            return 0;
        }

        function freq_hz(value) {
            var dot = value.indexOf('.'),
                dec = parseInt(value.substring(0, dot), 10),
                frac = scale_frac(dec, value.substring(dot + 1));
                freq = add_dec_frac(dec, frac);

            if(dot === -1) {
                return add_dec_frac(parseInt(value, 10), 0);
            }

            return freq;
        }

        function convFreq(arg) {
            if(typeof(arg) === 'number') {
                convFreq(arg.toString());
            }

            var freq = freq_hz(arg.trim());

            var valid_freqs = [
                [  1800000,   2000000],
                [  3500000,   4000000],
                [  5332000,   5332000],
                [  5348000,   5348000],
                [  5358500,   5358500],
                [  5373000,   5373000],
                [  5405000,   5405000],
                [  7000000,   7300000],
                [ 10100000,  10150000],
                [ 14000000,  14350000],
                [ 18068000,  18168000],
                [ 21000000,  21450000],
                [ 24890000,  24990000],
                [ 28000000,  29700000],
                [ 50000000,  54000000],
                [144000000, 148000000],
                [420000000, 450000000],
            ];

            if(valid_freqs.some( function(f) {
                return (freq >= f[0] && freq <= f[1]);
            })) {
                return parseInt(freq, 10);
            }

            return null;
        }

        function formatFreq(freq) {
            var m, k, h;

            if(typeof(freq) === 'number') {
                return formatFreq(freq.toString());
            }

            if(freq.length === 7) {
                m = freq.substring(0,1);
                k = freq.substring(1,4);
                h = freq.substring(4);
            }
            else if(freq.length === 8) {
                m = freq.substring(0,2);
                k = freq.substring(2,5);
                h = freq.substring(5);
            }
            else if(freq.length === 9) {
                m = freq.substring(0,3);
                k = freq.substring(3,6);
                h = freq.substring(6);
            }

            return trim_zeros(m + k + '.' + h);
        }

        return {
            convFreq: convFreq,
            formatFreq: formatFreq,
        };
    })(),
};

export default rig_commands;

