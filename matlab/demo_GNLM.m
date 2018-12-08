clear all; clc; close all;

clip = load('clip_T1.mat');
noisy = clip.noisy;
guide = clip.guide; 
L     = clip.L;
enl_area = clip.enl_area;

fprintf('run GNLM1 ...\n');
startime = tic();
out1 = guidedNLMeans(noisy, L, guide,  256, 0.002); %GNLM1
timealg1 = toc(startime);
reg1 = out1(enl_area.y1:enl_area.y2, enl_area.x1:enl_area.x2);
enl1 = (mean(reg1(:).^2)^2)/var(reg1(:).^2);
fprintf('GNLM1 is done\n');

fprintf('run GNLM2 ...\n');
startime = tic();
out2 = guidedNLMeans(noisy, L, guide, 1521, 0.004); %GNLM2
timealg2 = toc(startime);
reg2 = out2(enl_area.y1:enl_area.y2, enl_area.x1:enl_area.x2);
enl2 = (mean(reg2(:).^2)^2)/var(reg2(:).^2);
fprintf('GNLM2 is done\n');

figure();
subplot(2,2,1); imshow(uint8(guide));  title('Giude');
subplot(2,2,2); imshow(noisy,[0,255]); title('Noisy');
subplot(2,2,3); imshow(out1,[0,255]);  title(sprintf('GNLM1 ENL=%.1f',enl1));
subplot(2,2,4); imshow(out2,[0,255]);  title(sprintf('GNLM2 ENL=%.1f',enl2));
