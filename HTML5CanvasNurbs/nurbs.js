// nurbs.js
// Nolan Check
// Created 3/9/2012

var Nurbs = (function () {

    cubicNurbsTo = function (context, cp0x, cp0y, w0, cp1x, cp1y, w1, cp2x, cp2y, w2, x, y, w3) {
        context.moveTo(cp0x, cp0y);
        var NUM_STEPS = 256;
        for (var i = 0; i < NUM_STEPS; ++i) {
            var t = i / (NUM_STEPS - 1);
            var denom = w0 * (1 - t) * (1 - t) * (1 - t) + 3 * w1 * (1 - t) * (1 - t) * t + 3 * w2 * (1 - t) * t * t + w3 * t * t * t;
            var xt = (w0 * cp0x * (1 - t) * (1 - t) * (1 - t) + 3 * w1 * cp1x * (1 - t) * (1 - t) * t + 3 * w2 * cp2x * (1 - t) * t * t + w3 * x * t * t * t) / denom;
            var yt = (w0 * cp0y * (1 - t) * (1 - t) * (1 - t) + 3 * w1 * cp1y * (1 - t) * (1 - t) * t + 3 * w2 * cp2y * (1 - t) * t * t + w3 * y * t * t * t) / denom;
            context.lineTo(xt, yt);
        }
    };

    return {
        cubicNurbsTo: cubicNurbsTo
    };

})();