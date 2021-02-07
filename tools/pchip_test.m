close all
clear
clc

%%

data = [0 0;
   0.012 270.708;
   0.018 356.898;
   0.024 392.102;
   0.033 377.535;
   0.062 371.465;
   0.121 413.953;
   0.263 449.157;
   0.453 467.367;
   0.527 492.859;
   0.627 501.357;
   0.701 504.999;
   0.787 509.854;
   0.802 541.417;
   0.834 526.85;
   0.941 528.064;
   1.047 541.417;
   1.083 532.919;
   1.142 537.775;
   1.293 537.775;
   1.396 519.566;
   1.494 520.78;
   1.633 512.282;
   1.666 495.287;
   1.725 491.645;
   1.814 474.65;
   1.899 455.227;
   1.97 439.446;
   2.03 437.018;
   2.071 433.376;
   2.089 415.167;
   2.133 356.898;
   2.169 267.067;
   2.21 179.663;
   2.249 88.618;
   2.266 41.274;
   2.311 0.0];

time   = data(:,1);
thrust = data(:,2);

pp = pchip(time, thrust);

timeEval   = linspace(time(1), time(end), 1e3);
thrustEval = ppval(pp, timeEval);

%------------%
pydata;
%------------%

lnw = 2;

figure
hold on
plot(time, thrust, '-o', 'LineWidth', lnw)
plot(timeEval, thrustEval, 'LineWidth', lnw)
plot(timeEval, thrustPy, 'LineWidth', lnw)
hold off

legend('Original', 'MATLAB: pchip', 'GNU GSL: steffen')
grid on
box on
ax = gca;
ax.FontSize = 14;

figure
hold on
plot(timeEval, 100*(thrustEval'-thrustPy)./thrustPy)
hold off

box on
grid on
title('% Deviation')