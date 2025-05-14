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
    console.log('value', value);
    let dot = value.indexOf('.');
    if(dot === -1) {
	    return add_dec_frac(parseInt(value, 10), 0);
    } else {
        console.log('dot', dot);
        let dec = parseInt(value.substring(0, dot), 10);
        console.log('dec', dec);
        let frac = scale_frac(dec, value.substring(dot + 1));
        console.log('frac', frac);
        let freq = add_dec_frac(dec, frac);
        console.log('freq', freq);
    }

    return freq;
}

function convFreq(arg) {
    if(typeof(arg) === 'number') {
	    return convFreq(arg.toString());
    }

    let freq = freq_hz(arg.trim());

    let valid_freqs = [
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
    let m, k, h;

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

export default { convFreq, formatFreq };
