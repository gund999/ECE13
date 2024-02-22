% Given data
X_vals = [0; 2; 4; 6; 8; 10; 12; 14; 16; 18; 20; 22; 24; 26; 28; 30; 32; 34; 36; 38; 40; ...
     42; 44; 46; 48; 50; 52; 54; 56; 58; 60; 62; 64; 66; 68; 70; 72; 74; 76; ...
     78; 80; 82; 84; 86; 88; 90; 92];

Y = [130; 140; 200; 300; 500; 590; 692; 787; 920; 1015; 1134; 1285; 1415; ...
     1496; 1635; 1727; 1872; 2011; 2102; 2190; 2322; 2440; 2584; 2667; ...
     2830; 2976; 3044; 3136; 3300; 3365; 3511; 3647; 3744; 3857; 3980; ...
     4131; 4222; 4340; 4456; 4544; 4679; 4830; 4900; 5026; 5166; 5277; ...
     5417];
% Create a column vector of ones
onesColumn = ones(length(X_vals), 1);

% Combine the ones column to the original array
X = [X_vals, onesColumn];


% Calculate parameters using linear regression formula
abMatrix = X\Y;

a = abMatrix(1);
b = abMatrix(2);

display(a);
display(b);

% Make linear regression line
y = (a * X_vals) + b;

% Plot
plot(X_vals, Y, 'o-', 'LineWidth', 1.5);
hold on;
plot(X_vals, y, 'r-', 'LineWidth', 1.5);

title('Y vs X');
xlabel('X: Distance in cm');
ylabel('Y: Time in us');
legend('Original Data', 'Fitted Line');
grid on;
