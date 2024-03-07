%% Test 2 (BURSTING)
clear
time_max = 12*60*60;
b1 = zeros(time_max,1);
b1(37952:end) = 1;

% BATHROOM 1:
% - TOILETS: 
%   - 4s on x7
%   - [911, 13020, 20561, 28663, 37194]
% - SINKS:
%   - 30-40s on ([31, 38, 33, 39, 30])
%   - 80% COLD
%   - [924, 13033, 20574, 28676, 37207]
% - SHOWER: 20-30m on
%   - 20-30m on ([20])
%   - 20% HOT, 60% COLD
%   - [3287]

t1_f = zeros(time_max,1);
t1_f([911:911+4, 13020:13020+4, 20561:20561+4. 28663:28663+4, 37194:37194+4]) = 1;

sk1_h = zeros(time_max,1);
sk1_c = zeros(time_max,1);
sk1_c([924:924+31, 13033:13033+38, 20574:20574+33, 28676:28676+39, 37207:37207+30]) = 0.8;

sh1_h = zeros(time_max,1);
sh1_c = zeros(time_max,1);
sh1_h([3287:3287+1200]) = 0.2;
sh1_c([3287:3287+1200]) = 0.6;

% BATHROOM 2:
% - TOILETS: 
%   - 2s on x7
%   - [745, 12062, 21292, 29566, 32697, 38270]
% - SINKS:
%   - 20-40s on ([38, 14, 23, 20, 24, 10])
%   - 50% COLD, 50% HOT
%   - [758, 12075, 21305, 29579, 32710, 38283]
% - SHOWER: 20-30m on
%   - 10-15m on ([12])
%   - 50% COLD, 50% HOT
%   - [3365]

t2_f = zeros(time_max,1);
t2_f([745:745+3, 12062:12062+3, 21292:21292+3. 29566:29566+3, 32697:32697+3, 38270:38270+3]) = 1;

sk2_h = zeros(time_max,1);
sk2_c = zeros(time_max,1);
sk2_h([758:758+38, 12075:12075+14, 21305:21305+23, 29579:29579+20, 32710:32710+24, 38283:38283+10]) = 0.5;
sk2_c([758:758+38, 12075:12075+14, 21305:21305+23, 29579:29579+20, 32710:32710+24, 38283:38283+10]) = 0.5;

sh2_h = zeros(time_max,1);
sh2_c = zeros(time_max,1);
sh2_h([3365:3365+720]) = 0.5;
sh2_c([3365:3365+720]) = 0.5;

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