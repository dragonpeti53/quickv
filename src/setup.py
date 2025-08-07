from setuptools import setup, Extension

module = Extension('quickv',
                   sources=['quickv.c'])

setup(
    name='quickv',
    version='0.1.8',
    description='QuicKV is a fast, embedded key-value store database, built for Python, with no dependencies. ',
    #long_description=open('README.md').read(),
    #long_description_content_type='text/markdown',
    author='Your Name',
    author_email='you@example.com',
    url='https://github.com/dragonpeti53/quickv',
    ext_modules=[module],
    classifiers=[
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)
