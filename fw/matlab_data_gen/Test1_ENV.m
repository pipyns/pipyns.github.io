%% Test 1 (NO BURSTING)

time_max = 12*60*60;
b1 = zeros(time_max,1);

% BATHROOM 1:
% - TOILETS: 
%   - 3s on x7
%   - [4213, 12031, 18220, 23675, 27318, 35196, 39556]
% - SINKS:
%   - 30-60s on ([31, 55, 38, 33, 39, 51, 30])
%   - 100% COLD
%   - [4226, 12044, 18233, 23688, 27331, 35209, 39569]
% - SHOWER: 20-30m on
%   - 20-30m on ([27])
%   - 70% HOT
%   - [37504]

t1_f = zeros(time_max,1);
t1_f([4213:4213+3, 12031:12031+3, 18220:18220+3. 23675:23675+3, 27318:27318+3, 35196:35196+3, 39556:39556+3]) = 1;

sk1_h = zeros(time_max,1);
sk1_c = zeros(time_max,1);
sk1_c([4226:4226+31, 12044:12044+55, 18233:18233+38, 23688:23688+33, 27331:27331+39, 35209:35209+51, 39569:39569+30]) = 1;

sh1_h = zeros(time_max,1);
sh1_c = zeros(time_max,1);
sh1_h([37504:37504+1620]) = 0.7;
sh1_c([37504:37504+1620]) = 0.3;

% BATHROOM 2:
% - TOILETS: 
%   - 5s on x7
%   - [654, 11924, 21158, 29363, 32603, 38213]
% - SINKS:
%   - 20-40s on ([38, 14, 23, 20, 24, 10])
%   - 50% COLD, 10% HOT
%   - [667, 11937, 21171, 29376, 32616, 38226]
% - SHOWER: 20-30m on
%   - 10-15m on ([14])
%   - 50% COLD, 50% HOT
%   - [526]

t2_f = zeros(time_max,1);
t2_f([410:410+3, 11924:11924+3, 21158:21158+3. 29363:29363+3, 32603:32603+3, 38213:38213+3]) = 1;

sk2_h = zeros(time_max,1);
sk2_c = zeros(time_max,1);
sk2_h([423:423+38, 11937:11937+14, 21171:21171+23, 29376:29376+20, 32616:32616+24, 38226:38226+10]) = 0.1;
sk2_c([423:423+38, 11937:11937+14, 21171:21171+23, 29376:29376+20, 32616:32616+24, 38226:38226+10]) = 0.5;

sh2_h = zeros(time_max,1);
sh2_c = zeros(time_max,1);
sh2_h([526:526+840]) = 0.5;
sh2_c([526:526+840]) = 0.5;

b1 = timeseries(b1);
t1_f = timeseries(t1_f);
sk1_h = timeseries(sk1_h);
sk1_c = timeseries(sk1_c);
sh1_h = timeseries(sh1_h);
sh1_c = timeseries(sh1_c);
t2_f = timeseries(t2_f);
sk2_h = timeseries(sk2_h);
sk2_c = timeseries(sk2_c);
sh2_h = timeseries(sh2_h);
sh2_c = timeseries(sh2_c);