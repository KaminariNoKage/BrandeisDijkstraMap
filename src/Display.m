%Written by J. Storer, 11/1/2011.

%This program takes as input a .txt file of x,y coordinates
%of the vertices of a computed route on the cropped labeled Brandeis Map.
%Each line of the input file is two integers separated by a space
%that are the number of pixels to the right and down from the upper left corner.

%To run this program, open MatLab, do "cd xxx" where xxx is the directory
%for the map project, open this file, and click on the run icon.

%Input file names:
MapFile   = 'BrandeisMapLabeledCropped.jpg';
RouteFile = 'RouteCropped.txt';

%Display cropped map:
Map = imread(MapFile);
imshow(Map,'Border','tight');
hold on

%Load the path data:
P = load(RouteFile);

%Make the plot orgin the upper left corner:
axis ij

%Plot the route as a magenta line that is 8 pixels thick:
plot([P(:,1)],[P(:,2)],'m','LineWidth',8);
