#!/usr/bin/env python
import os, sys, shutil, socket, getopt, subprocess

paths = {
  'data': '/Users/ayla/devel/dhs_batch/U308',
  'tools': '/Users/ayla/devel/Seg3D_DHS/tools',
  'hmtscript': '/Users/ayla/devel/Seg3D_DHS'
}

def seg3d_connect(port, size):
  clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  clientsocket.connect(("localhost", port))
  data = clientsocket.recv(size)
  print "Received: [{}]".format(data)
  return clientsocket

def seg3d_disconnect(clientsocket, size):
  clientsocket.send("exit\r\n")
  data = clientsocket.recv(size)
  clientsocket.close();
  print "Received: [{}]".format(data)


def seg3d_command(clientsocket, size, command):
  clientsocket.send(command)
  data = clientsocket.recv(size)
  #print "Received: [{}]".format(data)
  return data

def ensure_directories(directories):
  for d in directories:
    if not os.path.exists(d):
        os.makedirs(d)

def update_filepath(filename, strings):
  newfilename = filename
  for key, value in strings.items():
    newfilename = newfilename.replace(key, value)
  #print(newfilename)
  return newfilename

def main(argv):
  size = 1024
  port = 9000

  try:
    opts, args = getopt.getopt(argv, "p:", ["port="])

  except getopt.GetoptError as e:
    print 'Exception: {0}.'.format(e.msg)
    print 'batch_processing.py [-p,--port=<port>]'
    sys.exit(2)

  for opt, arg in opts:
    if opt in ("-s", "--port"):
      port = int(arg)

  im = os.path.join(paths['data'], 'im')
  im_raw = os.path.join(im, 'raw')
  im_gray = os.path.join(im, 'gray')
  # expecting TIFF files
  im_raw_files = [os.path.join(im_raw, f) for f in os.listdir(im_raw) if os.path.isfile(os.path.join(im_raw, f)) and f.lower().endswith('.tif')]

  # test
  im_raw_test = os.path.join(im, 'raw_test')
  im_gray_test = os.path.join(im, 'gray_test')
  # expecting TIFF files
  im_raw_test_files = [os.path.join(im_raw_test, f) for f in os.listdir(im_raw_test) if os.path.isfile(os.path.join(im_raw_test, f)) and f.lower().endswith('.tif')]

  # setup directories
  im_cropped = os.path.join(paths['data'], 'im_cropped')
  im_cropped_gray = os.path.join(im_cropped, 'gray')
  im_cropped_chm = os.path.join(im_cropped, 'chm')
  im_cropped_blur = os.path.join(im_cropped, 'chm-blur')
  # test
  im_cropped_gray_test = os.path.join(im_cropped, 'gray_test')
  im_cropped_chm_test = os.path.join(im_cropped, 'chm_test')
  im_cropped_blur_test = os.path.join(im_cropped, 'chm-blur_test')

  truth = os.path.join(paths['data'], 'truth')
  truth_raw = os.path.join(truth, 'raw')
  truth_region = os.path.join(truth, 'region')
  truth_raw_files = [os.path.join(truth_raw, f) for f in os.listdir(truth_raw) if os.path.isfile(os.path.join(truth_raw, f)) and f.lower().endswith('.png')]

  # setup directories
  truth_cropped = os.path.join(paths['data'], 'truth_cropped')
  truth_cropped_region = os.path.join(truth_cropped, 'region')
  # TODO: not sure if needed
  #truth_cropped_gray = os.path.join(truth_cropped, 'gray')

  ensure_directories( (im_gray, im_cropped, im_cropped_gray, im_cropped_chm, im_cropped_blur, im_gray_test, im_cropped_gray_test, im_cropped_chm_test, im_cropped_blur_test, truth_region, truth_cropped, truth_cropped_region) )

  rgbToGray2D_tool = os.path.join(paths['tools'], 'RGBToGray2D')
  padImage_tool = os.path.join(paths['tools'], 'PadImage')
  blur_image_tool = os.path.join(paths['tools'], 'blur_image')

  # clean up filenames
  for f in truth_raw_files:
    # shorten filename, remove spaces, change directory
    newfile = update_filepath(f, { 'U3O8_': '', 'Particle ': 'p', truth_raw: truth_region })
    os.rename(f, newfile)

  raw_files = im_raw_files + im_raw_test_files

  # Raw data RGB files to grayscale (ITK tool) - RGBToGray2D
  for f in raw_files:
    # shorten filename, remove spaces, change directory
    newfile = update_filepath(f, { 'U3O8_': '', 'Particle ': 'p', im_raw_test: im_gray_test, im_raw: im_gray, '.tif': '.mha' })
    subprocess.check_call([rgbToGray2D_tool, f, newfile, 'float'])

  clientsocket = seg3d_connect(port, size)
  # add some variables to Seg3D's interpreter
  retval = seg3d_command(clientsocket, size, "im_gray='{}'\r\n".format(im_gray))
  retval = seg3d_command(clientsocket, size, "im_cropped_gray='{}'\r\n".format(im_cropped_gray))
  retval = seg3d_command(clientsocket, size, "im_cropped_chm='{}'\r\n".format(im_cropped_chm))
  retval = seg3d_command(clientsocket, size, "im_gray_test='{}'\r\n".format(im_gray_test))
  retval = seg3d_command(clientsocket, size, "im_cropped_gray_test='{}'\r\n".format(im_cropped_gray_test))
  retval = seg3d_command(clientsocket, size, "im_cropped_chm_test='{}'\r\n".format(im_cropped_chm_test))
  retval = seg3d_command(clientsocket, size, "truth_region='{}'\r\n".format(truth_region))
  retval = seg3d_command(clientsocket, size, "truth_cropped_region='{}'\r\n".format(truth_cropped_region))
  #retval = seg3d_command(clientsocket, size, "exec(open('/Users/ayla/devel/Seg3D_DHS/scripts/seg3d_filters.py').read())\r\n")
  seg3d_disconnect(clientsocket, size)

  # cleanup
  truth_cropped_files = [os.path.join(truth_cropped_region, f) for f in os.listdir(truth_cropped_region) if os.path.isfile(os.path.join(truth_cropped_region, f)) and f.lower().endswith('.png')]
  for f in truth_cropped_files:
    newfile = update_filepath(f, { 'Crop_': '', '_label-00': '' })
    os.rename(f, newfile)

  boundary_files = [os.path.join(im_cropped_chm, f) for f in os.listdir(im_cropped_chm) if os.path.isfile(os.path.join(im_cropped_chm, f)) and f.lower().endswith('.mha')]
  boundary_files_test = [os.path.join(im_cropped_chm_test, f) for f in os.listdir(im_cropped_chm_test) if os.path.isfile(os.path.join(im_cropped_chm_test, f)) and f.lower().endswith('.mha')]
  boundary_files_all = boundary_files + boundary_files_test

  # Boundary files to blurred boundary files
  for f in boundary_files_all:
    # shorten filename, remove spaces, change directory
    newfile = update_filepath(f, { im_cropped_chm: im_cropped_blur })
    subprocess.check_call([blur_image_tool, "--inputImage={}".format(f), "--outputImage={}".format(newfile), "--sigma=1", "--kernelWidth=3"])

  # Call GLIA script
  subprocess.check_call(['python', '/Users/ayla/devel/Seg3D_DHS/hmt_test_batch.py', im_cropped_chm, im_cropped_chm_test, im_cropped_blur, im_cropped_blur_test, truth_cropped_region ])

  # Pad to original dimensions
  # ./PadImage gm_rep1_A_004.png gm_rep1_A_004_padded.png 0 0 0 59 0
  #for f in raw_files:
  #  # shorten filename, remove spaces, change directory
  # newfile =
  #  subprocess.check_call([padImage_tool f, newfile, '0', '0', '0', '59', '0'])

if __name__ == "__main__":
  main(sys.argv[1:])
