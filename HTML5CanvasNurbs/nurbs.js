// nurbs.js
// Nolan Check
// Created 3/9/2012

var Nurbs = (function () {

    cubicNurbsTo = function (context, cp0x, cp0y, w0, cp1x, cp1y, w1, cp2x, cp2y, w2, x, y, w3) {
        context.moveTo(cp0x, cp0y);
        var NUM_STEPS = 256;
        for (var i = 0; i < NUM_STEPS; ++i) {
            var t = i / (NUM_STEPS - 1);
			var t2 = t*t;
			var t3 = t2*t;
			var u = 1-t;
			var u2 = u*u;
			var u3 = u2*u;
            var denom = w0*u3 + 3*w1*u2*t + 3*w2*u*t2 + w3*t3;
            var xt = (w0*cp0x*u3 + 3*w1*cp1x*u2*t + 3*w2*cp2x*u*t2 + w3*x*t3) / denom;
            var yt = (w0*cp0y*u3 + 3*w1*cp1y*u2*t + 3*w2*cp2y*u*t2 + w3*y*t3) / denom;
            context.lineTo(xt, yt);
        }
    };

    return {
        cubicNurbsTo: cubicNurbsTo
    };

})();