belgium_osm(1,:)  = [0.034666 0.022752 0.020443 0.01314 0.011199 0.009173 0.014224];
belgium_osm(2,:)  = [1.127658 0.61118 0.527908 0.328314 0.276032 0.246801 0.259111];
belgium_osm(3,:)  = [0.325235 0.45816 0.511548 0.525401 0.65676	0.575105 0.529546];
belgium_osm(4,:)  = [0.408234 0.834361 0.907804 1.129239 1.322111 0.957454 1.412737];
				
com_Youtube(1,:) = [14.114383 8.779148 8.024981 5.183534 4.952887 4.254761 4.98567];
com_Youtube(2,:) = [11.361568 7.116321 6.889981 6.176771 5.97861	6.017408 6.362033];
com_Youtube(3,:) = [10.885222 11.810333 11.770153 13.909444 16.443477 17.82571 19.507392];
com_Youtube(4,:) = [15.697133 16.354739 16.788621 18.553813 19.944654 20.39752 30.364081];
				
dblp_2010(1,:) = [0.223444 0.298646 0.312497 0.372597 0.380927 0.400591 0.402327];
dblp_2010(2,:) = [0.485463 0.518053 0.544418 0.673179 0.845523 0.968334 1.319437];
dblp_2010(3,:) = [0.367943 0.401437 0.441289 0.468826 0.490228 0.49128 0.503244];
dblp_2010(4,:) = [0.392149 0.420376 0.43624 0.420185 0.497917	0.541389 0.566438];
				
mycielskian13(1,:) = [5.878607 4.18836 2.4813 1.355077 1.019938 0.767201 0.770393];
mycielskian13(2,:) = [4.395219 2.2409 2.008297 1.086644 0.802314 0.620592 0.666034];
mycielskian13(3,:) = [2.803191 2.769288 2.880769 3.051306 3.353793 3.211121 3.278095];
mycielskian13(4,:) = [3.827866 4.038416 4.216509 5.027114 6.190566 6.63717 4.121751];
				
NACA0015(1,:) = [0.459631 0.529403 0.53389 0.635606 0.65949 0.67474 0.57717];
NACA0015(2,:) = [0.9989	0.742436 0.871536 1.165466 1.468721 1.697627 1.796968];
NACA0015(3,:) = [1.143856 1.578928 1.525148 1.539863 1.709866 1.703061 1.711837];
NACA0015(4,:) = [1.035767 0.959249 1.154263 1.203759 1.524997 1.37541 1.549325];

n = [2 4 5 10 15 20];

close all;

figure;
for i = 1:4
    plot(n, belgium_osm(i,1:6), 'Marker', 'x');
    hold on;
end
legend('v3 Cilk', 'v3 openMP', 'v4 Cilk', 'v4 openMP', 'Location', 'northwest');
title('Execution time for belgium_osm.mtx', 'Interpreter', 'none');
xlabel('Number of workers/threads');
ylabel('Execution time (sec)');

figure;
for i = 1:4
    plot(n, com_Youtube(i,1:6), 'Marker', 'x');
    hold on;
end
legend('v3 Cilk', 'v3 openMP', 'v4 Cilk', 'v4 openMP', 'Location', 'best');
title('Execution time for com_Youtube.mtx', 'Interpreter', 'none');
xlabel('Number of workers/threads');
ylabel('Execution time (sec)');

figure;
for i = 1:4
    plot(n, dblp_2010(i,1:6), 'Marker', 'x');
    hold on;
end
legend('v3 Cilk', 'v3 openMP', 'v4 Cilk', 'v4 openMP', 'Location', 'best');
title('Execution time for dblp_2010.mtx', 'Interpreter', 'none');
xlabel('Number of workers/threads');
ylabel('Execution time (sec)');

figure;
for i = 1:4
    plot(n, mycielskian13(i,1:6), 'Marker', 'x');
    hold on;
end
legend('v3 Cilk', 'v3 openMP', 'v4 Cilk', 'v4 openMP', 'Location', 'best');
title('Execution time for mycielskian13.mtx', 'Interpreter', 'none');
xlabel('Number of workers/threads');
ylabel('Execution time (sec)');

figure;
for i = 1:4
    plot(n, NACA0015(i,1:6), 'Marker', 'x');
    hold on;
end
legend('v3 Cilk', 'v3 openMP', 'v4 Cilk', 'v4 openMP', 'Location', 'best');
title('Execution time for NACA0015.mtx', 'Interpreter', 'none');
xlabel('Number of workers/threads');
ylabel('Execution time (sec)');