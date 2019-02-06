function [ o, s] = calibrateAccelerometer( accX, accY, accZ )
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% calibrateAcelerometer calculate the acceleration calibration parameters
%
% Parameters
%    accX, accY, accZ - vector containing the accelerometers samples for
%    each axis (1xn vector).
%
% Return 
%    o - the offset for each axis (1x3 vector)
%    s - the sensitivity for each axis (1x3 vector)
%
% The calibration parameters should be used as:
%   accCalibrated = (accMeasured - o) / s
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

firstRow = ones(size(accX));

accSamples = [ firstRow; accX; accY; accZ ];
Asystem = accSamples * accSamples';

E = -(accX.^2 + accY.^2+ accZ.^2);

Bsystem = E * accSamples';

resp = mldivide(Asystem, Bsystem');

o = resp(2:4)/(-2);

r = sqrt(sum(o.^2) - resp(1));

accX = (accX - o(1)).^2;
accY = (accY - o(2)).^2;
accZ = (accZ - o(3)).^2;

accSamples2 = [ accX; accY; accZ ];
Asystem2 = accSamples2 * accSamples2';

Bsystem2 = [sum(accX); sum(accY); sum(accZ)]';

resp2 = mldivide(Asystem2, Bsystem2');

s = 1./sqrt(resp2);

end

