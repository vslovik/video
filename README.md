#### Seam carving for video retargeting 
##### SPM course project, 2017

##### Requirements

OpenCV version 3.2 and video codecs installed

For automated provisioning with Ansible see:
https://github.com/vslovik/videobox

##### To install

1. Unpack the archive or use git repository:

`$ git clone https://github.com/vslovik/video.git`

`$ cd video/parallel`

2. Install FastFlow:

`$ svn co https://svn.code.sf.net/p/mc-fastflow/code/ fastflow`
`$ svn update`

3. Correct path to FastFlow in Makefile if needed

##### To compile

`$ cd video/parallel`
`$ make clean`
`$ make`

##### To run

`$ ./video path/to/videofile <nv> <nh> <nw>`

nv — number of vertical seams to remove, nh — number of horisontal seams to remove, 
nw — number of workers

##### Usage examples

`$ ./video ../data/videoplayback.mp4 0 100 10`

`$ ./video ../data/Megamind.mp4 0 100 10`