RE = 6378;
HTS = 470;
HIR = 780;
betaIR = 126; % degrees
betaTS = 120;
aTS = RE + HTS;
aIR = RE + HIR;


theta = linspace(0,4.74);

gamma = (asind(aTS * sind(theta) ./ sqrt(aTS^2 + aIR^2 - 2*aTS*aIR*cosd(theta))) + theta)- betaTS/2;

plot(theta, gamma)
hold on
plot(theta, zeros(1,length(theta)), '-k')
xlabel('$ \theta$','interpreter','latex')
ylabel('$ \gamma$','interpreter','latex')
%{
fun = @(x)geometry(x, aIR, aTS, betaIR);

x0 = [500,0];
x = fsolve(fun,x0);

x(2) = x(2);


function F = geometry(x, aIR, aTS, betaIR)

F(1) = x(1)^2 - aIR^2 - aTS^2 + 2*aTS*aIR*cosd(x(2));
F(2) = x(1) * sind(betaIR/2) - aTS*sind(x(2));
end
%}

