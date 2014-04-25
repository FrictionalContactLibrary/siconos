try:
    import Siconos.Kernel as SK
    import Siconos.Numerics as N

except (ImportError):
    print 'Could not import Siconos.* module'

import numpy

class MyR(SK.FirstOrderNonLinearR):
    ## \brief Constructor
    #
    # \param  is a  (optional)
    def __init__(self, C, B):
        SK.FirstOrderNonLinearR.__init__(self)
        self.setBPtr(B)
        self.setCPtr(C)
        return


    def computeh(self,time, x, l, y):
        print 'call computeh'
        print(x)
        print(y)
        #numpy.copyto(y, numpy.dot(self.C(), x))
        y[:] = self.C().dot(x)[:]
        print('computeh done')
        pass

    def computeg(self,time, x, l, R):
        print 'call computeg'
        print(l)
        print(R)
        #numpy.copyto(R, numpy.dot(self.B(), l))
        R[:] = self.B().dot(l)[:]
        print(R)
        print('computeg done')
        pass

    def computeJachx(self,time, x, l, C):
        #self.setJachxPtr(C) not exiting ??
        #numpy.copyto(self._C,self.jachx() ) version 1.7
        print('call computeJachx')
        print(C)
        C[:] = numpy.eye(2)[:]

#        numpy.copyto(SK.getMatrix(C), self.C())
        #print 'self.jachx()', self.jachx()
        pass

    def computeJacglambda(self,time, x, l, B):
        print('call computeJacglambda')
        print(B)
#        numpy.copyto(SK.getMatrix(B), self.B())
        #print 'self.jacglambda() = ', self.jacglambda()
        #self.setJachglambdaPtr(self._B) not callable in that form ?
        pass

    def computeJacgx(self,time, x, l, K):
        print('call computeJacgx')
        print(K)
#        numpy.copyto(SK.getMatrix(B), self.B())
        #print 'self.jacglambda() = ', self.jacglambda()
        #self.setJachglambdaPtr(self._B) not callable in that form ?
        pass

    def computeJachlambda(self, time, x, l, D):
        print('call computeJachlambda')
        print(D)
#        numpy.copyto(SK.getMatrix(B), self.B())
        #print 'self.jacglambda() = ', self.jacglambda()
        #self.setJachglambdaPtr(self._B) not callable in that form ?
        pass




