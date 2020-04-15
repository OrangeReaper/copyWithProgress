# copyWithProgress
copyWithProgress is a QT QThread subclass which is designed to run in a LINUX environment. It *Copies* or *Moves* a single file and emits a *progress* signal intended for a QProgress widget while doing so. 

If an error occurs the operation stops and a *fault* signal is emitted with a message describing the fault.

The class will not copy empty files and can detect the following fault conditions

 * Source File does not exist
 * Failure to delete source file (on move to different volume)
 * Source file is empty
 * Target file/location is invalid
 * Insufficient space on target 
